#pragma once
#include <pcontacts.h>

const int BLOB_COUNT = 5;
class ParticleCollision : public ParticleContactGenerator
{
public:
    /**
     * Holds a pointer to the particles we're checking for collisions with.
     */
    Particle* particle[BLOB_COUNT];
 
    virtual unsigned addContact(ParticleContact* contact,unsigned limit) const;
    bool checkCollision(Particle* particle_i, Particle* particle_j) const;
};