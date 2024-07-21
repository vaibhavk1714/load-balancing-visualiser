#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#define M_PI 3.14159265358979323846

const int windowWidth = 800;
const int windowHeight = 600;


struct Server {
    float load;   
    float height; 
    float maxCapacity; 
    float x;      
};

struct Client {
    float x, y; 
    float load; 
};

std::vector<Server> servers;
std::vector<Client> clients;

const float initialMaxCapacity = 1.0f;

const float lowLoadThreshold = 0.33f;
const float mediumLoadThreshold = 0.66f;

bool drawing = false;
float startX, startY, endX, endY;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
}

void renderServer(float x, float y, const Server& server) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(1.0f, server.height, 1.0f); 

    if (server.load <= lowLoadThreshold) {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    else if (server.load <= mediumLoadThreshold) {
        glColor3f(1.0f, 0.65f, 0.0f);
    }
    else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }

    glutSolidCube(0.1);
    glPopMatrix();
}

void renderClient(const Client& client) {
    glPushMatrix();
    glTranslatef(client.x, client.y, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); 
    glutSolidSphere(0.05, 20, 20);
    glPopMatrix();
}

void renderArrow(float x1, float y1, float x2, float y2) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    float angle = atan2(y2 - y1, x2 - x1);
    float arrowHeadLength = 0.05f;
    glBegin(GL_TRIANGLES);
    glVertex2f(x2, y2);
    glVertex2f(x2 - arrowHeadLength * cos(angle + M_PI / 6), y2 - arrowHeadLength * sin(angle + M_PI / 6));
    glVertex2f(x2 - arrowHeadLength * cos(angle - M_PI / 6), y2 - arrowHeadLength * sin(angle - M_PI / 6));
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    for (const auto& server : servers) {
        renderServer(server.x, 0.0f, server);
    }

    for (const auto& client : clients) {
        renderClient(client);
    }

    if (drawing) {
        renderArrow(startX, startY, endX, endY);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void addClient() {
    float xPos = -0.75f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 1.5f));
    clients.push_back({ xPos, -0.5f, 0.1f });
}

void increaseLoad(Server& server, float amount) {
    server.load += amount;
    if (server.load > 1.0f) {
        server.load = 1.0f;
    }
    server.height = server.load * server.maxCapacity;

    if (server.height >= server.maxCapacity && servers.size() < 5) {
        servers.push_back({ 0.0f, 0.1f, initialMaxCapacity, servers.size() * 0.2f - 0.5f });
    }
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'c': 
        addClient();
        break;
    case 27: 
        exit(0);
        break;
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            drawing = true;
            startX = (x / (float)windowWidth) * 2.0f - 1.0f;
            startY = -((y / (float)windowHeight) * 2.0f - 1.0f);
        }
        else if (state == GLUT_UP) {
            drawing = false;
            endX = (x / (float)windowWidth) * 2.0f - 1.0f;
            endY = -((y / (float)windowHeight) * 2.0f - 1.0f);

            float minDistance = 100.0f;
            int nearestServerIndex = -1;
            for (size_t i = 0; i < servers.size(); ++i) {
                float dx = endX - servers[i].x;
                float dy = endY;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestServerIndex = i;
                }
            }

            if (nearestServerIndex != -1) {
                increaseLoad(servers[nearestServerIndex], 0.1f);
            }
        }
    }
}

void motion(int x, int y) {
    if (drawing) {
        endX = (x / (float)windowWidth) * 2.0f - 1.0f;
        endY = -((y / (float)windowHeight) * 2.0f - 1.0f);
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    servers.push_back({ 0.0f, 0.1f, initialMaxCapacity, -0.5f });

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Scaling and Client Visualizer");

    initOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
