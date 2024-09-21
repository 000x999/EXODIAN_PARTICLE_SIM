#ifndef PARTICLE_H
#define PARTICLE_H

#include "raylib.h"
#include "Vec2.h"
#include "HashGrid.h"
#include <immintrin.h>
#include <cstdint>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>

#define SPRING_CONSTANT 0.1
#define DAMPING_FACTOR 0.35f

class Particle {

private: 

	Vec2 m_pos; 
	Vec2 m_prevPos; 
	std::vector<Particle*> m_particleVector;
	//Static variables for the update loop
	bool m_isSelected = false; 
	Color m_color;

public: 
	
	//Public to be accessed by GUI controls
	Vec2 p_force = { 0.0f,0.0f }; 
	bool p_isShowingCollisions = false;
	bool p_isGridActive = false;
	bool p_toggleCircleLines = false;
	bool p_showOutlines = false;
	
	//Initialization of the spatial hashing grid
	Hash<Particle> p_hashGrid;
	float p_radius = 5.0f;
	float p_mass = p_radius;
	float p_particleCount = 0.0f;

	Particle(const Vec2& pos_in, const Vec2& force_in, float mass_in, float radius_in, Color color_in); 
	Particle() = default;  
	~Particle() = default;
	void DrawParticle(const Particle& particle) const; 
	void UpdateParticle(Particle& particle, float dt);
	void UpdateLoop(float dt, float decay_in);
	void ParticleCollision(Particle& particle, Particle& OtherParticle);
	void WorldCollision(Particle& particle, float decay_in);
	void AddParticle(); 
	void MoveParticles(Particle& p, Particle& other, float decay_in)const; 
	bool IsColliding(Particle& particle, Particle& OtherParticle);
	void ColorizeParticles(float maxspeed);
	void DrawCount();
	Vec2 GetPos()const { return m_pos; };
	void SetPos(Vec2 pos_in) { m_pos = pos_in; }; 
	Vec2 GetPrev() const { return m_prevPos; };
	void SetPrev(Vec2 prev_in) { m_prevPos = prev_in; };
	bool GetState() const { return m_isSelected; };
	void SetState(bool state_in) { m_isSelected = state_in; }; 
	float GetDistance(Particle& particle, Particle& OtherParticle);
	float GetRadius() const { return p_radius; };
	float fast_rsqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
	float fast_sqrt(float x) { return x * fast_rsqrt(x); }
	float GetParticleCount() const { return p_particleCount; };
	void SetParticleCount(float count_in) { p_particleCount = count_in;  };
	float GetParticleSpeed();
	void Clear();
	Color InterpolateColor(Color c1, Color c2, float t); 
	Color GetHeatWaveColor(float t);  
};

#endif // !PARTICLE_H
