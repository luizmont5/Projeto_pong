#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cmath>
#include <windows.h> // Para sons no Windows
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Constantes
const int windowWidth = 800;
const int windowHeight = 600;
const int paddleWidth = 20;
const int paddleHeight = 100;
const float ballRadius = 10.0f;
const float ballSpeedIncrement = 0.1f; // Incremento de velocidade
const float initialBallSpeed = 0.5f;

// Variáveis globais
float paddle1Y = windowHeight / 2 - paddleHeight / 2;
float paddle2Y = windowHeight / 2 - paddleHeight / 2;
float ballX = windowWidth / 2;
float ballY = windowHeight / 2;
float ballSpeedX = initialBallSpeed;
float ballSpeedY = initialBallSpeed;
int score1 = 0, score2 = 0;
bool gamePaused = false;
bool ballReady = false;
int scoringPlayer = 0;

// Funções auxiliares para som
void playSound(const char *filename) {
    PlaySound(TEXT(filename), NULL, SND_ASYNC | SND_FILENAME);
}

// Desenho do placar
void drawText(const std::string &text, float x, float y) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Desenho do retângulo
void drawRect(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Atualização do jogo
void updateBall() {
    if (ballReady) return; // Bola não se move até ser lançada

    // Movimento da bola
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Colisão com o topo e a base
    if (ballY - ballRadius < 0 || ballY + ballRadius > windowHeight) {
        ballSpeedY = -ballSpeedY;
    }

    // Colisão com as barras
    if (ballX - ballRadius < paddleWidth && 
        ballY > paddle1Y && ballY < paddle1Y + paddleHeight) {
        ballSpeedX = fabs(ballSpeedX) + ballSpeedIncrement;
        ballSpeedX = fabs(ballSpeedX); // Garante direção
        playSound("bounce.wav");
    }
    if (ballX + ballRadius > windowWidth - paddleWidth && 
        ballY > paddle2Y && ballY < paddle2Y + paddleHeight) {
        ballSpeedX = -fabs(ballSpeedX) - ballSpeedIncrement;
        playSound("bounce.wav");
    }

    // Pontuação
    if (ballX - ballRadius < 0) {
        score2++;
        ballReady = true;
        scoringPlayer = 2;
        playSound("score.wav");
    }
    if (ballX + ballRadius > windowWidth) {
        score1++;
        ballReady = true;
        scoringPlayer = 1;
        playSound("score.wav");
    }
}

// Desenho da cena
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenha o campo
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRect(0, paddle1Y, paddleWidth, paddleHeight);
    drawRect(windowWidth - paddleWidth, paddle2Y, paddleWidth, paddleHeight);

    // Desenha a bola
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float theta = i * 3.14159 / 180;
        glVertex2f(ballX + ballRadius * cos(theta), ballY + ballRadius * sin(theta));
    }
    glEnd();

    // Desenha o placar
    drawText(std::to_string(score1), windowWidth / 2 - 50, windowHeight - 50);
    drawText(std::to_string(score2), windowWidth / 2 + 30, windowHeight - 50);

    glutSwapBuffers();
}

// Atualização do jogo
void update(int value) {
    if (!gamePaused) {
        updateBall();
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Movimentos das barras
void handleKeys(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        if (paddle1Y + paddleHeight < windowHeight) paddle1Y += 10;
        break;
    case 's':
        if (paddle1Y > 0) paddle1Y -= 10;
        break;
    case ' ':
        gamePaused = !gamePaused;
        break;
    case 13: // Enter
        if (ballReady) {
            ballReady = false;
            ballX = (scoringPlayer == 1) ? paddleWidth + ballRadius : windowWidth - paddleWidth - ballRadius;
            ballY = (scoringPlayer == 1) ? paddle1Y + paddleHeight / 2 : paddle2Y + paddleHeight / 2;
            ballSpeedX = (scoringPlayer == 1) ? initialBallSpeed : -initialBallSpeed;
            ballSpeedY = initialBallSpeed;
        }
        break;
    }
}

// Controles das setas
void handleSpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        if (paddle2Y + paddleHeight < windowHeight) paddle2Y += 10;
        break;
    case GLUT_KEY_DOWN:
        if (paddle2Y > 0) paddle2Y -= 10;
        break;
    }
}

// Configuração inicial
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
}

// Main
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Pong");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutSpecialFunc(handleSpecialKeys);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
