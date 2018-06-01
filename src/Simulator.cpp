#include "core/Application.h"
#include <iostream>
#include <cstdlib> 
#include <ctime> 


using namespace std;

struct Particule
{
    vec4 position;
    vec4 velocity;
    vec4 size;
    Particule(vec3 pos, vec3 vel, vec3 siz)
    {
        position = vec4(pos[0], pos[1], pos[2], 1.0f);
        velocity = vec4(vel[0], vel[1], vel[2], 1.0f);
        size = vec4(siz[0], siz[1], siz[2], 1.0f);
    }
};

class Simulator: public Application
{
private:
    Program* program = NULL;
    Program* compute = NULL;
    Renderer* renderer = NULL;
    Renderer* linesRenderer = NULL;
    Computer* computer = NULL;
    Buffer* particules = NULL;
    Buffer* velocities = NULL;
    Buffer* lines = NULL;
    int countOfParticules = 50000;
    int rendererVar = 8;
    
public:
    Simulator(int argc, char* argv[]);
    void update(int time);
    void render();
    void setup();
    void teardown();
};

Simulator::Simulator(int argc, char* argv[]) : Application(argc, argv) {}

void Simulator::update(int elapsedTime)
{
    computer->compute(countOfParticules, 1, 1);
}

void Simulator::render()
{
     renderer->render(PRIMITIVE_POINTS, countOfParticules * rendererVar);
     linesRenderer->render(PRIMITIVE_LINES, 24);
}

void Simulator::setup()
{  
    vector<Particule> pp;
    srand((unsigned)time(0)); 
    int i;
    for( int a = 1; a < countOfParticules * rendererVar; a = a + 1 ) {
      pp.push_back(Particule(vec3(
      (rand()%10)+1.0f, 
      (rand()%18)+1.0f, 
      (rand()%10)+1.0f), 

      vec3(
      (rand()%3+rand()%4)+1.0f, 
      (rand()%3+rand()%4)+1.0f ,  
      (rand()%3+rand()%4)+1.0f), vec3(1)));
   }

    setClearColor(0.95f, 0.95f, 0.95f, 1.0f); 

    /*Particule pp[] = {
        Particule(vec3(1, 10, 1), vec3(0)),
        Particule(vec3(-1, 10, 1), vec3(0)),
        Particule(vec3(1, 10, -1), vec3(0)),
        Particule(vec3(-1, 10, -1), vec3(0))
    };*/

    /*vec3 parti[] = {
        vec3(1, 10, 1),
        vec3(-1, 10, 1),
        vec3(1, 10, -1),
        vec3(-1, 10, -1)
    };*/
    float limit = 10.0;
    float linesPoint[] = {
        -1*limit,-1*limit , -1*limit, 
         limit, -1*limit, -1*limit, 

        -1*limit,-1*limit , -1*limit,  
        -1*limit, limit , -1*limit, 
        
        limit, -1*limit, -1*limit, 
        limit, -1*limit, limit,

        limit, -1*limit, limit, 
        limit, limit, limit, 

        limit, -1*limit, -1*limit, 
        limit, limit, -1*limit,

        limit, limit, -1*limit,
        limit, limit, limit, 

        limit, limit, -1*limit,
        -1*limit, limit, -1*limit, 

        -1*limit, limit, -1*limit, 
        -1*limit, limit, limit,

        -1*limit, limit, limit,
        limit, limit, limit,

        -limit, -limit, -limit,
        -limit, -limit, limit,

        -limit, -limit, limit,
        limit, -limit, limit,

        -limit, -limit, limit,
        -limit, limit, limit,   
    };
    /* sizeof(): Size in byte of a particle
    * => Multiply the size with the number of particles to have the 
    *    size of all particles
    */
    particules = new Buffer(pp.data(), pp.size()*sizeof(Particule));
	
    lines = new Buffer(linesPoint, sizeof(linesPoint));

    /*vec3 veloc[] = {
        vec3(0),
        vec3(0),
        vec3(0),
        vec3(0)
    };

    velocities = new Buffer(4, 3, FLOAT, veloc);*/

    program = new Program();
    program->addShader(Shader::fromFile("shaders/perspective.vert"));
    program->addShader(Shader::fromFile("shaders/black.frag"));
    program->link();

    renderer = program->createRenderer();
    renderer->setVertexData("vertex", particules, TYPE_FLOAT, 0, 3, sizeof(Particule));

    linesRenderer = program->createRenderer();
    // Last param: Size between vertices in byte	
    linesRenderer->setVertexData("vertex", lines, TYPE_FLOAT, 0, 3, 3*sizeof(float));
    
    /* perspective(Zoom, Window_Screen,Near_plan,Far_plan) */
    mat4 projection = perspective(90.0f, 640.0/480.0, 0.1, 100);
   
   /* lookat() Params
    * 1 : CAM positon
    * 2 : Where the CAM looks at
    * 3 : Reference axis (y here because only one different from 0)
    */ 
    mat4 view = lookat(vec3(30, 10,6), vec3(0, 0, 0), vec3(0, 1, 0));

    renderer->setMatrix("projectionMatrix", projection);
    renderer->setMatrix("modelViewMatrix", view);

    linesRenderer->setMatrix("projectionMatrix", projection);
    linesRenderer->setMatrix("modelViewMatrix", view);

    compute = new Program();
    compute->addShader(Shader::fromFile("shaders/gravity.comp"));
    //compute->addShader(Shader::fromFile("shaders/weight.comp"));
    compute->link();

    computer = compute->createComputer();
    computer->setData(1, particules);
    //computer->setData(2, velocities);
}

void Simulator::teardown()
{
   
}

int main(int argc, char** argv)
{
    try 
    {
        Simulator app = Simulator(argc, argv); 
	    return app.run();
    }
	catch(Exception e)
    {
        cout << e.getMessage() << endl;
        return 1;
    }
}
