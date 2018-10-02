Write("Normal String")

NormalBulletData = {
	ReloadTime = 0.2,
	EffectLifetime = 5,
	Speed = 600,
}
ShotgunData = {
	ReloadTime = 0.8,
	EffectLifetime = 5,	
}
CanonballData = {
	ReloadTime = 1.5,
	EffectLifetime = 6,
	NumDeathBullet = 20,
}
LaserData = {
	ReloadTime = 0.2,
	EffectLifetime = 1,
	ShipRotationSpeed = 45,
	LineLength = 200,
}
LightningData = {
	ReloadTime = 1,
	EffectLifetime = 0.2,
	Range = 1000,
}

function UpdateNormalBullet(deltaSeconds, bullet)
	center = {}
	velocity = {}
	center[0] = Bullet.GetCenterX(bullet)
	center[1] = Bullet.GetCenterY(bullet)
	velocity[0] = Bullet.GetVelocityX(bullet)
	velocity[1] = Bullet.GetVelocityY(bullet)
	velocity[0] = velocity[0] * deltaSeconds
	velocity[1] =velocity[1] * deltaSeconds
	center[0] = center[0] + velocity[0]
	center[1] = center[1] + velocity[1]
	Bullet.SetCenter(bullet, center[0], center[1])

	timeToDie = Bullet.GetTimeToDie(bullet)
	timeToDie = timeToDie - deltaSeconds
	Bullet.SetTimeToDie(bullet, timeToDie)
end

function UpdateLaser(deltaSeconds, bullet)
	normalizedVelocity = {}
	shipCenter = {}
	newCenter = {}
	orientation = Game.GetShipOrientation()
	normalizedVelocity[0] = Vector.GetNormalizedVectorX(orientation)
	normalizedVelocity[1] = Vector.GetNormalizedVectorY(orientation)
	Write(normalizedVelocity[0])
	shipCenter[0] = Game.GetShipCenterX()
	shipCenter[1] = Game.GetShipCenterY()
	newCenter[0] = shipCenter[0] + (normalizedVelocity[0] * LaserData.LineLength)
	newCenter[1] = shipCenter[1] + (normalizedVelocity[1] * LaserData.LineLength)
	Laser.SetCenter(bullet, newCenter[0], newCenter[1])
	Laser.SetRotationAroundPoint(bullet, shipCenter[0], shipCenter[1], orientation)
end

function UpdateLightning(deltaSeconds, bullet)
	missileCenter = {}
	isShipNull = Game.IsShipNull()
	if isShipNull == 0 then
		closestMissile = Game.FindClosestMissile(LightningData.Range)
		isMissileNull = Missile.IsNull(closestMissile)
		if isMissileNull == 0 then
			missileCenter[0] = Missile.GetCenterX(closestMissile)
			missileCenter[1] = Missile.GetCenterY(closestMissile)
			Lightning.SetLineEnd(bullet, missileCenter[0], missileCenter[1])
			Lightning.SetIsAttached(bullet, true)
			missileHealth = Missile.GetHealth(closestMissile)
			Missile.SetHealth(closestMissile, missileHealth - 1)
			Lightning.CreateLightningShape(bullet)
		end
	end
end

function OnDeath(bullet)
	bulletType = Bullet.GetBulletType(bullet)
	if bulletType == "Canonball" then
		OnDeathCanonball(bullet)
	end
end

function OnDeathCanonball(bullet)
	normalizedVelocity = {}
	bulletVelocity = {}
	spawnPos = {}
	bulletCenter = {}
	
	isShipNull = Game.IsShipNull()
	if isShipNull == 0 then
		circleDegrees = 360
		for vertexIndex = 0, 19 do
			degreesThisVertex = (vertexIndex / CanonballData.NumDeathBullet) * circleDegrees

			normalizedVelocity[0] = Vector.GetNormalizedVectorX(degreesThisVertex)
			normalizedVelocity[1] = Vector.GetNormalizedVectorY(degreesThisVertex)

			bulletVelocity[0] = normalizedVelocity[0] * NormalBulletData.Speed
			bulletVelocity[1] = normalizedVelocity[1] * NormalBulletData.Speed

			Write ("hello")
			bulletCenter[0] = Bullet.GetCenterX(bullet)
			bulletCenter[1] = Bullet.GetCenterY(bullet)
			bulletRadius = Bullet.GetRadius(bullet)

			spawnPos[0] = bulletCenter[0] + (normalizedVelocity[0] * bulletRadius)
			spawnPos[1] = bulletCenter[1] + (normalizedVelocity[1] * bulletRadius)
			
			newBullet = Bullet.Create(bulletVelocity[0], bulletVelocity[1], spawnPos[0], spawnPos[1])
			Bullet.SetType(newBullet, "Normal")
			Game.AddBullet(newBullet)
		end
	end
end