#pragma once
#ifndef APOC3D_PARTICLESETTINGS_H
#define APOC3D_PARTICLESETTINGS_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2009-2018 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/Math/Color.h"
#include "apoc3d/Math/Vector.h"
#include "apoc3d/Graphics/GraphicsCommon.h"

using namespace Apoc3D::Graphics;
using namespace Apoc3D::Graphics::RenderSystem;
using namespace Apoc3D::Math;
using namespace Apoc3D::Core;

namespace Apoc3D
{
	namespace Graphics
	{
		/** Settings class describes all the tweakable options used
		 * to control the appearance of a particle system.
		 */
		class ParticleSettings
		{
		public:
			/* Maximum number of particles that can be displayed at one time. */
			int32 MaxParticles;

			/* How long these particles will last. */
			float Duration;

			/* If greater than zero, some particles will last a shorter time than others. */
			float DurationRandomness;

			/* Controls how much particles are influenced by the velocity of the object
			   which created them. You can see this in action with the explosion effect,
			   where the flames continue to move in the same direction as the source
			   projectile. The projectile trail particles, on the other hand, set this
			   value very low so they are less affected by the velocity of the projectile.
			*/
			float EmitterVelocitySensitivity;

			/* Range of values controlling how much X and Z axis velocity to give each
			   particle. Values for individual particles are randomly chosen from somewhere
			   between these limits.
			*/
			float MinHorizontalVelocity;
			float MaxHorizontalVelocity;

			/* Range of values controlling how much Y axis velocity to give each particle.
			   Values for individual particles are randomly chosen from somewhere between
			   these limits.
			*/
			float MinVerticalVelocity;
			float MaxVerticalVelocity;

			/* Direction and strength of the gravity effect. Note that this can point in any
			   direction, not just down! The fire effect points it upward to make the flames
			   rise, and the smoke plume points it sideways to simulate wind.
			*/
			Vector3 Gravity;

			/* Controls how the particle velocity will change over their lifetime. If set
			   to 1, particles will keep going at the same speed as when they were created.
			   If set to 0, particles will come to a complete stop right before they die.
			   Values greater than 1 make the particles speed up over time.
			*/
			float EndVelocity;

			/* Range of values controlling the particle color and alpha. Values for 
			   individual particles are randomly chosen from somewhere between these limits.
			*/
			Color4 MinColor;
			Color4 MaxColor;

			/* Range of values controlling how fast the particles rotate. Values for
			   individual particles are randomly chosen from somewhere between these
			   limits. If both these values are set to 0, the particle system will
			   automatically switch to an alternative shader technique that does not
			   support rotation, and thus requires significantly less GPU power. This
			   means if you don't need the rotation effect, you may get a performance
			   boost from leaving these values at 0.
			*/
			float MinRotateSpeed;
			float MaxRotateSpeed;

			/* Range of values controlling how big the particles are when first created.
			   Values for individual particles are randomly chosen from somewhere between
			   these limits.
			*/
			float MinStartSize;
			float MaxStartSize;

			/* Range of values controlling how big particles become at the end of their
			   life. Values for individual particles are randomly chosen from somewhere
			   between these limits.
			*/
			float MinEndSize;
			float MaxEndSize;

			float ZBias;

			ParticleSettings()
				: 
				MaxParticles(100),
				Duration(1.f),
				DurationRandomness(0),
				EmitterVelocitySensitivity(1),
				MinHorizontalVelocity(0),
				MaxHorizontalVelocity(0),
				MinVerticalVelocity(0),
				MaxVerticalVelocity(0),
				Gravity(Vector3::Zero),
				EndVelocity(1),
				MinRotateSpeed(0),
				MaxRotateSpeed(0),
				MinStartSize(100),
				MaxStartSize(100),
				MinEndSize(100),
				MaxEndSize(100),
				ZBias(0),
				MinColor(1.0f,1.0f,1.0f,1.0f),
				MaxColor(1.0f,1.0f,1.0f,1.0f)
			{
			}
		};

	}
}
#endif