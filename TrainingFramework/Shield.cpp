#include "stdafx.h"
#include "Shield.h"

Shield::Shield()
{
	SetState(&Shield::Idle);
}

Shield::Shield(Blueprint * blueprint, Vector3 pos, Vector3 scale, Vector3 rotation) : Shield()
{
	name = _strdup(blueprint->name);

	//Update transform
	UpdatePosition(pos.x, pos.y, pos.z);
	UpdateRotation(rotation.x, rotation.y, rotation.z);
	UpdateScale(scale.x, scale.y, scale.z);

	//Clone components
	for (vector<Component *>::iterator it = blueprint->componentList.begin(); it != blueprint->componentList.end(); ++it) {
		AddComponent((*it)->Clone());
	}
	Init();
}


Shield::~Shield()
{
	b2Body* tempBody = GetComponent<Collision2D>()->body;
	tempBody->GetWorld()->DestroyBody(tempBody);
}


void Shield::Init()
{
	shieldDuration = 200;
	b2Filter filter = GetComponent<Collision2D>()->body->GetFixtureList()->GetFilterData();
	//type of body
	filter.categoryBits = SHIELD;
	//collide with what
	filter.maskBits = PLAYER | BULLET_BLUE | BULLET_RED | BOSS | EXPLOSION | ITEM | BULLET_E | MOB_RED | MOB_BLUE | WALL | EXPLOSION | MOB;
	GetComponent<Collision2D>()->body->GetFixtureList()->SetFilterData(filter);

}

void Shield::Destroyed()
{
	InitDestroyed();
	GameManager::GetInstance()->addToRemovalList(this);
}

void Shield::InitDestroyed()
{
	GetComponent<Collision2D>()->body->GetFixtureList()->SetSensor(true);
}


void Shield::AddComponent(Component * comp)
{
	GameObject::AddComponent(comp);
	if (dynamic_cast<Animation *>(comp) != NULL) {
		comp->isActive = false;
		animeList.push_back((Animation*)comp);
	}
}

void Shield::PlayAnimation(int key)
{
	//Deactivate active animation and active this one if its not active
	if (!animeList.at(key)->isActive) {
		animeList.at(key)->isActive = true;
		if (activeAnimation != -1)
			animeList.at(activeAnimation)->isActive = false;
		activeAnimation = key;
	}
}

void Shield::Spawn()
{
	GameManager::GetInstance()->Spawn("spawn",
		SceneManager::GetInstance()->GetBlueprintByName("spawn"),
		Vector3(transform->position.x, transform->position.y, 2),
		Vector3(1.5, 1.5, 1.5),
		Vector3());
	SetState(&Shield::Idle);
}

void Shield::Idle()
{
	PlayAnimation(0);
}


void Shield::Update(float deltaTime)
{
	if (activeState != NULL)
		(this->*activeState)();


	AddToPosition(0.0f, 0.0f);


	GameObject::Update(deltaTime);
}

void Shield::checkCollision(GameObject * tempObj)
{
	if (strcmp(tempObj->name, "pBullet_red") == 0 
		|| strcmp(tempObj->name, "pBullet_blue") == 0
		|| strcmp(tempObj->name, "pBullet_blue_crit") == 0
		|| strcmp(tempObj->name, "pBullet_red_crit") == 0) {
		((Bullet*)tempObj)->SetState(&Bullet::Despawn);
	}
}


