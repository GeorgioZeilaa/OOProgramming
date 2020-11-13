/*
 * The Blob demo.
 *
 */
#include <gl/glut.h>
#include "blob.h"
#include "coreMath.h"
#include "pworld.h"
#include "platform.h"
#include <stdio.h>
#include <cassert>
#include <quadtree.h>

const Vector2 Vector2::GRAVITY = Vector2(0,-9.81);
/**
 * Platforms are two dimensional: lines on which the 
 * particles can rest. Platforms are also contact generators for the physics.
 */

bool ParticleCollision::checkCollision(Particle* particle_i, Particle* particle_j) const
{
    float x = particle_i->getPosition().x - particle_j->getPosition().x;
    float y = particle_i->getPosition().y - particle_j->getPosition().y;

    float squared = (x*x)+(y*y);
    float distance = sqrt(squared);
    
    float radius_Total = particle_i->getRadius() + particle_j->getRadius();

    if (distance < radius_Total)
    {
        return true;
    }
    else
    {
        return false;
    }
}

unsigned ParticleCollision::addContact(ParticleContact* contact, unsigned limit) const
{
    const static float restitution = 1.0f;
    unsigned used = 0;

    //To make sure not to go over the max contact limits.
    if (used >= limit) return used;

    //Create the quadtree
    QuadTree* tree = new QuadTree(0, 0, 800, 600, 0, 2);
    
    //Pass all the particles to populate quadtree.
    for (int i = 0; i < BLOB_COUNT; i++)
    {
        tree->AddObject(particle[i]);
    }

    //Traverse the quadtree to populate a vector of particles.
    tree->PreOrder(tree);

    //To check if particles are found then to loop only through those particles.
    if (tree->particle.size() > 0)
    {
        for (int i = 0; i < tree->particle.size(); i++)
        {
            for (int j = 0; j < tree->particle.size(); j++)
            {
                if (checkCollision(tree->particle[i], tree->particle[j]) == true && i != j)
                {
                    // We have a collision
                    Vector2 toParticle = tree->particle[i]->getPosition();
                    contact->contactNormal = toParticle.unit();
                    contact->restitution = restitution;
                    contact->particle[0] = tree->particle[i];
                    contact->particle[1] = tree->particle[j];
                    contact->penetration = tree->particle[i]->getRadius() - toParticle.magnitude();
                    used++;
                    contact++;
                    cout << "Particle Contact collision" << " used = " << used << endl;
                }
            }
        }
        //Brute Force Method.
        //for (int j = 0; j < BLOB_COUNT; j++)
        //{
        //    if (checkCollision(i, j) == true && i != j)
        //    {
        //        Vector2 toParticle = particle[i]->getPosition();
        //        // We have a collision
        //        contact->contactNormal = toParticle.unit();
        //        contact->restitution = restitution;
        //        contact->particle[0] = particle[i];
        //        contact->particle[1] = particle[j];
        //        contact->penetration = particle[i]->getRadius() - toParticle.magnitude();
        //        used++;
        //        contact++;
        //    }
        //}

    }
    tree->~QuadTree();
    return used;
}

unsigned Platform::addContact(ParticleContact *contact, unsigned limit) const
{
	//const static float restitution = 0.8f;
	const static float restitution = 1.0f;
	unsigned used = 0;

    //To make sure not to go over the max contact limits.
    if (used >= limit) return used;

    // Check for penetration
    Vector2 toParticle = particle->getPosition() - start;
    Vector2 lineDirection = end - start;

    float projected = toParticle * lineDirection;
    float platformSqLength = lineDirection.squareMagnitude();
	float squareRadius = particle->getRadius()*particle->getRadius();
		 
    if (projected <= 0)
    {
        // The blob is nearest to the start point
        if (toParticle.squareMagnitude() < squareRadius)
        {
            // We have a collision
            contact->contactNormal = toParticle.unit();
            contact->restitution = restitution;
            contact->particle[0] = particle;
            contact->particle[1] = 0;
            contact->penetration = particle->getRadius() - toParticle.magnitude();
            used ++;
            contact ++;
            cout << "Particle Contact" << " used = " << used << endl;
        }
    }
    else if (projected >= platformSqLength)
    {
        // The blob is nearest to the end point
        toParticle = particle->getPosition() - end;
        if (toParticle.squareMagnitude() < squareRadius)
		{
            // We have a collision
            contact->contactNormal = toParticle.unit();
            contact->restitution = restitution;
            contact->particle[0] = particle;
            contact->particle[1] = 0;
            contact->penetration = particle->getRadius() - toParticle.magnitude();
            used ++;            
            contact ++;
            cout << "Particle Contact" << " used = " << used << endl;
        }
    }
    else
    {
        // the blob is nearest to the middle.
        float distanceToPlatform = toParticle.squareMagnitude() - projected*projected / platformSqLength;
        if (distanceToPlatform < squareRadius)
        {
            // We have a collision
            Vector2 closestPoint = start + lineDirection*(projected/platformSqLength);

            contact->contactNormal = (particle->getPosition()-closestPoint).unit();
			contact->restitution = restitution;
            contact->particle[0] = particle;
            contact->particle[1] = 0;
			contact->penetration = particle->getRadius() - sqrt(distanceToPlatform);
	        used ++;
            contact ++;
            cout << "Particle Contact" << " used = " << used << endl;
        }
    }
    return used;
}

// Method definitions
Blob::Blob():world(100, 10000)
{
	width = 800; height = 600; 
	nRange = 100.0;
    int tempposition = 0;

    for (int i = 0; i < BLOB_COUNT; i++)
    {
        collision[i] = new ParticleCollision;
    }
    for (int i = 0; i < BLOB_COUNT; i++)
    {
        // Create the blob storage on the heap.
        blob[i] = new Particle;

        // Create the platform
        platform = new Platform;

        //define the start and end of the platform for calculation purposes
        platform->start = Vector2(-50.0, 0.0);
        platform->end = Vector2(50.0, 10.0);

        //Make sure the platform knows which particle it should collide with.
        platform->particle = blob[i];

        //To add the platform to the contactgenerator vector.
        world.getContactGenerators().push_back(platform);

        //To add all possible particles that can collide.
        for (int j = 0; j < BLOB_COUNT; j++)
        {
            collision[j]->particle[i] = blob[i];
        }
        //Adding collisions that can occur to generate contacts vector.
        world.getContactGenerators().push_back(collision[i]);

        // Create the blob
        blob[i]->setPosition(tempposition, 90.0);
        blob[i]->setRadius(5);
        blob[i]->setVelocity(tempposition, 0);
        //blob[i]->setDamping(0.9);
        blob[i]->setAcceleration(Vector2::GRAVITY * 20.0f);

        blob[i]->setMass(50.0f);
        blob[i]->clearAccumulator();
        world.getParticles().push_back(blob[i]);
        tempposition += 11;
    }
}

Blob::~Blob()
{
    //For cleaning purposes of memory.
    for (int i = 0; i < BLOB_COUNT; i++)
    {
        delete blob[i];
        delete collision[i];
    }
    delete platform;
}

void Blob::display()
{
    //Creating the visuals.
    Application::display();

    const Vector2 &p0 = platform->start;
    const Vector2 &p1 = platform->end;

    glBegin(GL_LINES);
    glColor3f(0,1,1);
    glVertex2f(p0.x, p0.y);
    glVertex2f(p1.x, p1.y);
    glEnd();
	
    glColor3f(1,0,0); 
    for (int i = 0; i < BLOB_COUNT; i++)
    {
        const Vector2& p = blob[i]->getPosition();
        glPushMatrix();
        glTranslatef(p.x, p.y, 0);
        glutSolidSphere(blob[i]->getRadius(), 12, 12);
        glPopMatrix();
    }

    glutSwapBuffers();
    
}

void Blob::box_collision_resolve(Particle* particle)
{
    Vector2 position = particle->getPosition();
    Vector2 velocity = particle->getVelocity();

    float radius = particle->getRadius();
    float w = Application::width;
    float h = Application::height;
    if (position.x > w - radius || position.x < -w + radius)
        particle->setVelocity(-velocity.x, velocity.y);

    if (position.y > h - radius || position.y < -h + radius)
        particle->setVelocity(velocity.x, -velocity.y);
}

bool Blob::out_of_box_test(Particle* particle)
{
    Vector2 position = particle->getPosition();
    Vector2 velocity = particle->getVelocity();
    float radius = particle->getRadius();

    if ((position.x > Application::width - radius) || (position.x < -Application::width + radius)) return true;

    if ((position.y > Application::height - radius) || (position.y < -Application::height + radius)) return true;

    return false;
}

void Blob::out_of_box_resolve(Particle* particle)
{
    Vector2 position = particle->getPosition();
    Vector2 velocity = particle->getVelocity();
    float radius = particle->getRadius();

    if (position.x > Application::width - radius)		position.x = Application::width - radius;
    else if (position.x < -Application::width + radius) position.x = -Application::width + radius;

    if (position.y > Application::height - radius)		position.y = Application::height - radius;
    else if (position.y < -Application::height + radius)position.y = -Application::height + radius;

    particle->setPosition(position.x, position.y);
}

void Blob::update()
{
    // Recenter the axes
	float duration = timeinterval/1000;
    // Run the simulation
    world.runPhysics(duration);
    float g = 9.81;

    for (int i = 0; i < BLOB_COUNT; i++)
    {
        float radius = blob[i]->getRadius();
        Vector2 velocity = blob[i]->getVelocity();
        float dragCoeff = 0.005 * radius * radius * velocity.magnitude() * velocity.magnitude();
        Vector2 dragForce = velocity;
        dragForce.normalise();
        dragForce *= -dragCoeff;

        blob[i]->addForce(Vector2(0, -g * blob[i]->getMass()));
        blob[i]->addForce(dragForce);

        box_collision_resolve(blob[i]);
        if (out_of_box_test(blob[i])) out_of_box_resolve(blob[i]);
    }

    Application::update();
}

const char* Blob::getTitle()
{
    return "Blob Demo";
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new Blob();
}