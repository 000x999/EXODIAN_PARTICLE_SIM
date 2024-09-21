#include "Particle.h"

Particle::Particle(const Vec2& pos_in, const Vec2& force_in, float mass_in, float radius_in, Color color_in) 
    :
    m_pos(pos_in), 
    p_force(force_in),
    p_mass(mass_in),
    p_radius(radius_in),
    m_color(color_in) 
    { m_prevPos = m_pos; }

void Particle::DrawParticle(const Particle& particle) const {
    //Outlining method when hovering over particles
       //Disabled for now since it eats up too many frames
       //I have it so theyre always buffered but only drawn when hovering over particles
    Color DrawColor = particle.GetState() ? YELLOW : particle.m_color;
    uint32_t DrawRadius = particle.GetState() ? static_cast<uint32_t> (particle.p_radius)
        : static_cast<uint32_t>(particle.p_radius); 
   /* if (ShowOutlines) {
        DrawCircleLinesV({ particle->pos.x, particle->pos.y }, particle->radius, DrawColor);
    }*/
    DrawCircleV({ particle.m_pos.x, particle.m_pos.y }, particle.p_radius, particle.m_color); 
}

void Particle::UpdateParticle(Particle& particle, float dt) {
        float conv = 1 / particle.p_mass;
        Vec2 LocalAccel = { particle.p_force.x * conv, particle.p_force.y * conv };
        Vec2 LocalPrev = particle.GetPos();
        Vec2 LocalPos = particle.GetPos();
        
        LocalPos.x = 2.0f * particle.m_pos.x - particle.m_prevPos.x + LocalAccel.x * (dt * dt);
        LocalPos.y = 2.0f * particle.m_pos.y - particle.m_prevPos.y + LocalAccel.y * (dt * dt);

        float velx = (particle.m_pos.x - particle.m_prevPos.x);
        float VelocityThreshold = 0.0099999f;
        if (std::fabs(velx) < VelocityThreshold) {
            velx = 0;
        }

        LocalPrev.x = LocalPos.x - velx;
        LocalPrev.y = particle.GetPos().y;
        particle.SetPos(LocalPos);
        particle.SetPrev(LocalPrev);

        float MaxSpeed = 10.0f;
        particle.ColorizeParticles(MaxSpeed);
}

void Particle::UpdateLoop(float dt, float decay_in) {
    p_hashGrid.Clear();
    AddParticle();
    DrawCount();

    for (auto& particle : m_particleVector) {
        p_hashGrid.AddToHash(*particle);
        particle->UpdateParticle(*particle, dt);

        //Fetching of all potential collisions
        auto PotentialCollisions = p_hashGrid.GetPotentialCollisions(*particle);
        for (auto& OtherHash : PotentialCollisions) {
            Particle& OtherParticle = OtherHash.get();
            particle->MoveParticles(*particle, OtherParticle, decay_in);
            //Solves the collisions by passing in the fetched valid particles from the PotentialCollisions vector
            particle->ParticleCollision(*particle, OtherParticle);
            if (p_isShowingCollisions) {
                DrawLineEx({ particle->m_pos.x, particle->m_pos.y }, { OtherParticle.m_pos.x, OtherParticle.m_pos.y }, 1.0f, particle->m_color);
            }
        }
       
        particle->WorldCollision(*particle, decay_in);
        if (p_isShowingCollisions) {
            if (p_toggleCircleLines) {
                DrawCircleLinesV({ particle->m_pos.x, particle->m_pos.y }, particle->p_radius, particle->m_color);
            }
        }
        else {
            particle->DrawParticle(*particle);
        }
        particle->Clear();
    }

    if (p_isGridActive) {
        p_hashGrid.DrawGrid();
    }
}

void Particle::ParticleCollision(Particle& particle, Particle& OtherParticle) {
    if (IsColliding(particle, OtherParticle) && &particle != &OtherParticle) {
        float distance = GetDistance(particle, OtherParticle);

        if (distance > 0) {
            float conv = 1 / distance;
            float Normalized_dx = (particle.m_pos.x - OtherParticle.m_pos.x) * conv;
            float Normalized_dy = (particle.m_pos.y - OtherParticle.m_pos.y) * conv;
            float ParticleOverlap = particle.p_radius + OtherParticle.p_radius - distance;
            float springForce = SPRING_CONSTANT * ParticleOverlap;
            float relativeVelX = (particle.m_pos.x - particle.m_prevPos.x) - (OtherParticle.m_pos.x - OtherParticle.m_prevPos.x);
            float relativeVelY = (particle.m_pos.y - particle.m_prevPos.y) - (OtherParticle.m_pos.y - OtherParticle.m_prevPos.y);
            float relativeVelDotNormal = relativeVelX * Normalized_dx + relativeVelY * Normalized_dy;

            float impulse = (springForce - DAMPING_FACTOR * relativeVelDotNormal) * 0.5f;

            particle.m_prevPos.x -= impulse * Normalized_dx;
            particle.m_prevPos.y -= impulse * Normalized_dy;
            OtherParticle.m_prevPos.x += impulse * Normalized_dx;
            OtherParticle.m_prevPos.y += impulse * Normalized_dy;

            float HalfOverlap = ParticleOverlap * 0.5f;
            particle.m_pos.x += (HalfOverlap * Normalized_dx);
            particle.m_pos.y += (HalfOverlap * Normalized_dy);
            OtherParticle.m_pos.x -= (HalfOverlap * Normalized_dx);
            OtherParticle.m_pos.y -= (HalfOverlap * Normalized_dy);
        }
    }
}

void Particle::WorldCollision(Particle& particle, float decay_in) {
    float velx = particle.m_pos.x - particle.m_prevPos.x;
    float vely = particle.m_pos.y - particle.m_prevPos.y;

    if (particle.m_pos.x - particle.p_radius < 0) {
        Vec2 TempPos = particle.GetPos();
        Vec2 TempPrev = particle.GetPrev();
        TempPos.x = 0 + particle.p_radius;
        TempPrev.x = TempPos.x + velx * decay_in;
        particle.SetPos(TempPos);
        particle.SetPrev(TempPrev);
    }
    else if ((particle.m_pos.x + particle.p_radius) > GetScreenWidth()) {
        Vec2 TempPos = particle.GetPos();
        Vec2 TempPrev = particle.GetPrev();
        TempPos.x = GetScreenWidth() - particle.p_radius;
        TempPrev.x = TempPos.x + velx * decay_in;
        particle.SetPos(TempPos);
        particle.SetPrev(TempPrev);
    }

    if (particle.m_pos.y - particle.p_radius < 0) {
        Vec2 TempPos = particle.GetPos();
        Vec2 TempPrev = particle.GetPrev();
        TempPos.y = 0 + particle.p_radius;
        TempPrev.y = TempPos.y + vely * decay_in;
        particle.SetPos(TempPos);
        particle.SetPrev(TempPrev);
    }
    else if ((particle.m_pos.y + particle.p_radius) > GetScreenHeight()) {
        Vec2 TempPos = particle.GetPos();
        Vec2 TempPrev = particle.GetPrev();
        TempPos.y = GetScreenHeight() - particle.p_radius;
        TempPrev.y = TempPos.y + vely * decay_in;
        particle.SetPos(TempPos);
        particle.SetPrev(TempPrev);
    }
}

void Particle::AddParticle() {
    static std::random_device rand;
    //Position damping so they don't all spawn from the same point
    static std::uniform_real_distribution<float> PosDampX(250, 650);
    static std::uniform_real_distribution<float> PosDampY(150, 650);
    
    if (m_particleVector.size() < static_cast<uint16_t>(p_particleCount)) {
        //This increases the particle spawnrate... at the cost of some temp frame loss
        //Ends up re-alligning itself ones the loop is complete
        for (uint16_t i = 0u; i < 15u; i++) {
            Vec2 pos_in = { PosDampX(rand), PosDampY(rand) };
            
            m_particleVector.emplace_back(new Particle(pos_in, p_force, p_mass, p_radius, m_color));
            p_hashGrid.AddToHash(*m_particleVector.back());
        }
    }
}

void Particle::MoveParticles(Particle& p, Particle& other, float decay_in) const {
    float dx = p.m_pos.x - GetMouseX();
    float dy = p.m_pos.y - GetMouseY();
    float distance = (dx * dx + dy * dy) *
                    _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(dx * dx + dy * dy)));
    float radius = p.p_radius * 11.0f;

    if (distance < radius) {
        p.SetState(true);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            float targetX = GetMouseX();
            float targetY = GetMouseY();
            //Finicky float to get right... this one isn't bad
            float stepFactor = 0.00455f;
            p.m_pos.x += (targetX - p.m_pos.x) * stepFactor;
            p.m_pos.y += (targetY - p.m_pos.y) * stepFactor;

            if (&p != &other) {
                float other_dx = p.m_pos.x - other.m_pos.x;
                float other_dy = p.m_pos.y - other.m_pos.y;
                float other_distance = (other_dx * other_dx + other_dy * other_dy) 
                                     * _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(other_dx * other_dx + other_dy * other_dy)));
                float min_distance = p.p_radius + other.p_radius;

                if (other_distance < min_distance && other_distance > 0.0f) {
                    float overlap = min_distance - other_distance;
                    float conv = 1 / other_distance;
                    other_dx *= conv;
                    other_dy *= conv;
                    
                    p.m_pos.x += other_dx * overlap * decay_in;
                    p.m_pos.y += other_dy * overlap * decay_in;
                    other.m_pos.x -= other_dx * overlap * decay_in;
                    other.m_pos.y -= other_dy * overlap * decay_in;
                }
            }
        }
    }
    else {
        p.SetState(false);
    }
}

bool Particle::IsColliding(Particle& particle, Particle& OtherParticle) {
    return (GetDistance(particle, OtherParticle)) < (particle.p_radius + OtherParticle.p_radius);
}

void Particle::ColorizeParticles(float maxspeed) {
    float speed = GetParticleSpeed();
    float conv = 1 / maxspeed;
    float FinalColor = speed * conv;
    m_color = GetHeatWaveColor(FinalColor);
}

void Particle::DrawCount() {
    int numOfParticles = m_particleVector.size();
    std::string countStr = std::to_string(numOfParticles);
    const char* countCStr = countStr.c_str();
    DrawText(countCStr, 942, 30, 22, RED);
}

float Particle::GetDistance(Particle& particle, Particle& OtherParticle) {
    return fast_sqrt((particle.m_pos.x - OtherParticle.m_pos.x) * (particle.m_pos.x - OtherParticle.m_pos.x) +
        (particle.m_pos.y - OtherParticle.m_pos.y) * (particle.m_pos.y - OtherParticle.m_pos.y));
}

float Particle::GetParticleSpeed() {
    //Intrinsic fast_sqrt messes it up for some reason? idk why lol
    //makes the particles flash in and out
    return sqrtf((m_pos.x - m_prevPos.x) * (m_pos.x - m_prevPos.x) + (m_pos.y - m_prevPos.y) * (m_pos.y - m_prevPos.y));
}

void Particle::Clear() {
    m_particleVector.clear();
}
 
Color Particle::InterpolateColor(Color c1, Color c2, float iColor) {
    if (iColor < 0.0f) iColor = 0.0f;
    if (iColor > 1.0f) iColor = 1.0f;

    unsigned char r = static_cast<unsigned char>((1.0f - iColor) * c1.r + iColor * c2.r);
    unsigned char g = static_cast<unsigned char>((1.0f - iColor) * c1.g + iColor * c2.g);
    unsigned char b = static_cast<unsigned char>((1.0f - iColor) * c1.b + iColor * c2.b);
    unsigned char a = static_cast<unsigned char>((1.0f - iColor) * c1.a + iColor * c2.a);

    Color result = { r, g, b, a };
    return result;
}

Color Particle::GetHeatWaveColor(float iColor) {
    Color blue = BLUE;
    Color green = GREEN;
    Color yellow = YELLOW;
    Color orange = ORANGE;
    Color red = RED;

    if (iColor <= 0.25f) {
        return InterpolateColor(blue, green, iColor * 4);
    }
    else if (iColor <= 0.5f) {
        return InterpolateColor(green, yellow, (iColor - 0.25f) * 4);
    }
    else if (iColor <= 0.75f) {
        return InterpolateColor(yellow, orange, (iColor - 0.5f) * 4);
    }
    else {
        return InterpolateColor(orange, red, (iColor - 0.75f) * 4);
    }
}
