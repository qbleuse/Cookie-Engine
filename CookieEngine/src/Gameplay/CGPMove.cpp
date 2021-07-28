#include "Gameplay/CGPMove.hpp"
#include "Core/Primitives.hpp"
#include "Core/Time.hpp"

using namespace Cookie::Core::Math;
using namespace Cookie::Resources;
using namespace Cookie::Gameplay;

void CGPMove::UpdatePushedCooldown(Resources::Map& map)
{
	if (state != CGPMOVE_STATE::E_PUSHED)
		return;

	pushedCooldownBeforeReturn -= Core::DeltaTime();

	if (pushedCooldownBeforeReturn < 0 && map.ApplyPathfinding(map.GetTile(trs->pos), map.GetTile(posBeforePushed)))
		SetPath(map.GetTile(posBeforePushed));
}
void CGPMove::UpdateReachGoalCooldown()
{
	if (state != CGPMOVE_STATE::E_REACH_GOAL)
		return;

	reachGoalCountdown -= Core::DeltaTime();

	if (reachGoalCountdown <= 0)
		state = CGPMOVE_STATE::E_STATIC;

}

void CGPMove::SetPath(Resources::Tile& lastWaypoint)
{
	waypoints.clear();
	Resources::Tile* currentTile = &lastWaypoint;

	//if Entity is Flying go as a straight line
	if (isFlying)
	{
		waypoints.emplace(waypoints.begin(), Core::Math::Vec3{ currentTile->pos.x, trs->pos.y, currentTile->pos.y });
		state = CGPMOVE_STATE::E_MOVING;
		return;
	}

	//if the Tile has no parent it mean it is the start Tile of the path so we don't add it 
	while (currentTile->parent != nullptr)
	{
		Core::Math::Vec3 currentWaypoint = { currentTile->pos.x, trs->pos.y, currentTile->pos.y };
		Core::Math::Vec3 parentWaypoint  = { currentTile->parent->pos.x, trs->pos.y, currentTile->parent->pos.y };

		if(waypoints.empty())
			waypoints.emplace(waypoints.begin(), currentWaypoint);
		//if the direction from currentWaypoint to next Waypoint is the same as the direction from parentWaypoint
		//to currentWaypoin then it's a straigth line so we don't add the currentWaypoint
		else if ((waypoints[0] - currentWaypoint).Normalize() != (currentWaypoint - parentWaypoint).Normalize())
			waypoints.emplace(waypoints.begin(), currentWaypoint);

		currentTile = currentTile->parent;
	}

	if (!waypoints.empty())
		state = CGPMOVE_STATE::E_MOVING;
}

void CGPMove::MoveTowardWaypoint()
{
	while (waypoints.size() != 0 && (waypoints[0] - trs->pos).Length() < 0.1)
	{
		waypoints.erase(waypoints.begin());

		//check if reach goal here to avoid to set the state each frame
		if (waypoints.size() == 0)
		{
			reachGoalCountdown = CGPMOVE_CD_BEFORE_STATIC;
			state = CGPMOVE_STATE::E_REACH_GOAL;
		}
	}

	if (waypoints.size() == 0 || state == CGPMOVE_STATE::E_WAITING)
		return;


	trs->pos += (waypoints[0] - trs->pos).Normalize() * (moveSpeed * Core::DeltaTime());
	trs->rot = Mat4::LookAt(trs->pos, waypoints[0], Vec3{0, 1, 0}).GetEuler();
	trs->trsHasChanged = true;
}

void CGPMove::PositionPrediction()
{
	//Empty for now but will help avoid colision ahead
}
void CGPMove::ResolveColision(CGPMove& other, Map& map)
{
	//Priority High
	if (state > other.state || (state == CGPMOVE_STATE::E_PUSHED && other.state == CGPMOVE_STATE::E_PUSHED))
	{
		if (other.state == CGPMOVE_STATE::E_STATIC)
		{
			other.state = CGPMOVE_STATE::E_PUSHED;
			other.posBeforePushed = other.trs->pos;
		}
		other.pushedCooldownBeforeReturn = CGPMOVE_CD_BEFORE_RETURN;

		Core::Math::Vec3 direction = Vec3{other.trs->pos.x - trs->pos.x, 0, other.trs->pos.z - trs->pos.z}.Normalize();
		other.trs->pos = trs->pos + direction * (trs->radius + other.trs->radius);
		other.trs->trsHasChanged = true;

		map.ClampPosInMapWithScale(*other.trs);
		map.ClampPosOutsideObstacleTile(*other.trs);
	}
	//Priority Medium need some fixes
	else if (state == CGPMOVE_STATE::E_MOVING && other.state == CGPMOVE_STATE::E_MOVING)
	{
		float overlapLength = (trs->radius + other.trs->radius) - (trs->pos - other.trs->pos).Length();

		Core::Math::Vec3 directionSelfToOther = Vec3{other.trs->pos.x - trs->pos.x, 0, other.trs->pos.z - trs->pos.z}.Normalize();
		Core::Math::Vec3 directionSelf = Vec3{waypoints[0].x - trs->pos.x, 0, waypoints[0].z - trs->pos.z}.Normalize();
		Core::Math::Vec3 directionOther = Vec3{other.waypoints[0].x - other.trs->pos.x, 0, other.waypoints[0].z - other.trs->pos.z}.Normalize();

		if (directionSelfToOther.Dot(directionSelf) > 0.9) // if they face each other
		{
			trs->pos       += Core::Math::Vec3{ directionSelf.z,  directionSelf.y,  -directionSelf.x }  * (overlapLength / 2);
			other.trs->pos += Core::Math::Vec3{ directionOther.z, directionOther.y, -directionOther.x } * (overlapLength / 2);
		}
		else // they collide side by side
		{
			//fix strange behavior for now
			//trs->pos += -directionSelfToOther * (overlapLength / 2);
			//other.trs->pos += directionSelfToOther * (overlapLength / 2);
			other.trs->pos += directionSelfToOther * (overlapLength);
		}

		trs->trsHasChanged = true;
		map.ClampPosInMapWithScale(*trs);
		map.ClampPosOutsideObstacleTile(*trs);
		other.trs->trsHasChanged = true;
		map.ClampPosInMapWithScale(*other.trs);
		map.ClampPosOutsideObstacleTile(*other.trs);

	}

}

void CGPMove::DrawPath(Render::DebugRenderer& debug)
{
	if (waypoints.size() == 0)
		return;

	debug.AddDebugElement(Core::Primitives::CreateLine({ trs->pos.x, 1, trs->pos.z }, { waypoints[0].x, 1, waypoints[0].z }, 0x00FFFF, 0x00FFFF));

	for (int i = 0; i < waypoints.size() - 1; ++i)
		debug.AddDebugElement(Core::Primitives::CreateLine(waypoints[i], waypoints[i + 1], 0x00FFFF, 0x00FFFF));
}