#pragma once
#include <pcontacts.h>
#include <iostream>
using namespace std;
class Platform : public ParticleContactGenerator
{
public:
    Vector2 start;
    Vector2 end;
    /**
     * Holds a pointer to the particles we're checking for collisions with.
     */
    Particle* particle;

    virtual unsigned addContact(ParticleContact* contact,unsigned limit) const;
};