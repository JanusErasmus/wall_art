#include "Boid.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float Boid::randomf() {
    return mapfloat(RANDOM(), 0, 255, -.5, .5);
}

Boid::Boid() {
    acceleration = PVector(0, 0);
    velocity = PVector(randomf(), randomf());
    location = PVector(0, 0);
    maxspeed = 1.5;
    maxforce = 0.05;
}

Boid::Boid(float x, float y) {
    acceleration = PVector(0, 0);
    velocity = PVector(randomf(), randomf());
    location = PVector(x, y);
    maxspeed = 1.5;
    maxforce = 0.05;
}

Boid::~Boid(){

}



void Boid::run(Boid boids [], uint8_t boidCount) {
    flock(boids, boidCount);
    update();
    // wrapAroundBorders();
    // render();
}

// Method to update location
void Boid::update() {
    // Update velocity
    velocity += acceleration;
    // Limit speed
    velocity.limit(maxspeed);
    location += velocity;
    // Reset acceleration to 0 each cycle
    acceleration *= 0;
}

void Boid::applyForce(PVector force) {
    // We could add mass here if we want A = F / M
    acceleration += force;
}

void Boid::repelForce(PVector obstacle, float radius) {
    //Force that drives boid away from obstacle.

    PVector futPos = location + velocity; //Calculate future position for more effective behavior.
    PVector dist = obstacle - futPos;
    float d = dist.mag();

    if (d <= radius) {
        PVector repelVec = location - obstacle;
        if (d != 0) { //Don't divide by zero.
            // float scale = 1.0 / d; //The closer to the obstacle, the stronger the force.
            repelVec.normalize();
            repelVec *= (maxforce * 7);
            if (repelVec.mag() < 0) { //Don't let the boids turn around to avoid the obstacle.
                repelVec.y = 0;
            }
        }
        applyForce(repelVec);
    }
}

// We accumulate a new acceleration each time based on three rules
void Boid::flock(Boid boids [], uint8_t boidCount) {
    PVector sep = separate(boids, boidCount);   // Separation
    PVector ali = align(boids, boidCount);      // Alignment
    PVector coh = cohesion(boids, boidCount);   // Cohesion
    // Arbitrarily weight these forces
    sep *= 1.5;
    ali *= 1.0;
    coh *= 1.0;
    // Add the force vectors to acceleration
    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
}

// Separation
// Method checks for nearby boids and steers away
PVector Boid::separate(Boid boids [], uint8_t boidCount) {
    PVector steer = PVector(0, 0);
    int count = 0;
    // For every boid in the system, check if it's too close
    for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
            continue;
        float d = location.dist(other.location);
        // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
        if ((d > 0) && (d < desiredseparation)) {
            // Calculate vector pointing away from neighbor
            PVector diff = location - other.location;
            diff.normalize();
            diff /= d;        // Weight by distance
            steer += diff;
            count++;            // Keep track of how many
        }
    }
    // Average -- divide by how many
    if (count > 0) {
        steer /= (float) count;
    }

    // As long as the vector is greater than 0
    if (steer.mag() > 0) {
        // Implement Reynolds: Steering = Desired - Velocity
        steer.normalize();
        steer *= maxspeed;
        steer -= velocity;
        steer.limit(maxforce);
    }
    return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
PVector Boid::align(Boid boids [], uint8_t boidCount) {
    PVector sum = PVector(0, 0);
    int count = 0;
    for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
            continue;
        float d = location.dist(other.location);
        if ((d > 0) && (d < neighbordist)) {
            sum += other.velocity;
            count++;
        }
    }
    if (count > 0) {
        sum /= (float) count;
        sum.normalize();
        sum *= maxspeed;
        PVector steer = sum - velocity;
        steer.limit(maxforce);
        return steer;
    }
    else {
        return PVector(0, 0);
    }
}

// Cohesion
// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
PVector Boid::cohesion(Boid boids [], uint8_t boidCount) {
    PVector sum = PVector(0, 0);   // Start with empty vector to accumulate all locations
    int count = 0;
    for (int i = 0; i < boidCount; i++) {
        Boid other = boids[i];
        if (!other.enabled)
            continue;
        float d = location.dist(other.location);
        if ((d > 0) && (d < neighbordist)) {
            sum += other.location; // Add location
            count++;
        }
    }
    if (count > 0) {
        sum /= count;
        return seek(sum);  // Steer towards the location
    }
    else {
        return PVector(0, 0);
    }
}

// A method that calculates and applies a steering force towards a target
// STEER = DESIRED MINUS VELOCITY
PVector Boid::seek(PVector target) {
    PVector desired = target - location;  // A vector pointing from the location to the target
    // Normalize desired and scale to maximum speed
    desired.normalize();
    desired *= maxspeed;
    // Steering = Desired minus Velocity
    PVector steer = desired - velocity;
    steer.limit(maxforce);  // Limit to maximum steering force
    return steer;
}

// A method that calculates a steering force towards a target
// STEER = DESIRED MINUS VELOCITY
void Boid::arrive(PVector target) {
    PVector desired = target - location;  // A vector pointing from the location to the target
    float d = desired.mag();
    // Normalize desired and scale with arbitrary damping within 100 pixels
    desired.normalize();
    if (d < 4) {
        float m = 0;//TODO map(d, 0, 100, 0, maxspeed);
        desired *= m;
    }
    else {
        desired *= maxspeed;
    }

    // Steering = Desired minus Velocity
    PVector steer = desired - velocity;
    steer.limit(maxforce);  // Limit to maximum steering force
    applyForce(steer);
    //Serial.println(d);
}

void Boid::wrapAroundBorders() {
    if (location.x < 0) location.x = MATRIX_WIDTH - 1;
    if (location.y < 0) location.y = MATRIX_HEIGHT - 1;
    if (location.x >= MATRIX_WIDTH) location.x = 0;
    if (location.y >= MATRIX_HEIGHT) location.y = 0;
}

void Boid::avoidBorders() {
    PVector desired = velocity;

    if (location.x < MATRIX_WIDTH / 4) desired = PVector(maxspeed, velocity.y);
    if (location.x >= MATRIX_WIDTH - MATRIX_WIDTH / 4) desired = PVector(-maxspeed, velocity.y);
    if (location.y < MATRIX_HEIGHT / 4) desired = PVector(velocity.x, maxspeed);
    if (location.y >= MATRIX_HEIGHT - MATRIX_HEIGHT / 4) desired = PVector(velocity.x, -maxspeed);

    if (desired != velocity) {
        PVector steer = desired - velocity;
        steer.limit(maxforce);
        applyForce(steer);
    }

    if (location.x < 0) location.x = 0;
    if (location.y < 0) location.y = 0;
    if (location.x >= MATRIX_WIDTH) location.x = MATRIX_WIDTH - 1;
    if (location.y >= MATRIX_HEIGHT) location.y = MATRIX_HEIGHT - 1;
}

bool Boid::bounceOffBorders(float bounce) {
    bool bounced = false;

    if (location.x >= MATRIX_WIDTH) {
        location.x = MATRIX_WIDTH - 1;
        velocity.x *= -bounce;
        bounced = true;
    }
    else if (location.x < 0) {
        location.x = 0;
        velocity.x *= -bounce;
        bounced = true;
    }

    if (location.y >= MATRIX_HEIGHT) {
        location.y = MATRIX_HEIGHT - 1;
        velocity.y *= -bounce;
        bounced = true;
    }
    else if (location.y < 0) {
        location.y = 0;
        velocity.y *= -bounce;
        bounced = true;
    }

    return bounced;
}

void Boid::render() {
    //// Draw a triangle rotated in the direction of velocity
    //float theta = velocity.heading2D() + radians(90);
    //fill(175);
    //stroke(0);
    //pushMatrix();
    //translate(location.x,location.y);
    //rotate(theta);
    //beginShape(TRIANGLES);
    //vertex(0, -r*2);
    //vertex(-r, r*2);
    //vertex(r, r*2);
    //endShape();
    //popMatrix();
    //backgroundLayer.drawPixel(location.x, location.y, CRGB::Blue);
}
