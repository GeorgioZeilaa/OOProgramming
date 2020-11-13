#pragma once

#include "app.h"
#include "pcontacts.h"
#include "pworld.h"
#include "platform.h"
#include "pcollision.h"

class Blob : public Application
{
    /**Used to store particles. */
    Particle* blob[BLOB_COUNT];

    /**Represents the number of particles for collision. */
    ParticleCollision* collision[BLOB_COUNT];

    /**Platform creation purposes. */
    Platform* platform;

    /**Creating the scene. */
    ParticleWorld world;

public:
    /** Creates a new demo object. */
    Blob();
    virtual ~Blob();

    /** Returns the window title for the demo. */
    virtual const char* getTitle();

    /** Display the particles. */
    virtual void display();

    /** Update the particle positions. */
    virtual void update();

    /**To detect collision of particles to the box border. */
    void box_collision_resolve(Particle* particle);

    /**To test if the particle is outside the box. */
    bool out_of_box_test(Particle* particle);

    /**To correct the particle that is trying to go outside the box region. */
    void out_of_box_resolve(Particle* particle);
};