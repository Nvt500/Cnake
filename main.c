#include <stdio.h>
#include <stdlib.h>
#include "input.h"


#define FPS ((int) 1000 / 5)
#define BORDER_SPRITE '#'
#define SNAKE_SPRITE 'O'
#define FRUIT_SPRITE '@'
#define WIDTH 40
#define HEIGHT 20


typedef struct Vector2 {
    int x;
    int y;
} vector2;


typedef struct Body body;
struct Body {
    body *former;
    body *next;
    body *last;
    vector2 position;
    vector2 direction;
};


void init_body(body *head, body *temp)
{
    temp->former = head->last;
    temp->next = NULL;
    temp->last = NULL;
    temp->position = (vector2) {
        head->last->position.x - head->last->direction.x, 
        head->last->position.y - head->last->direction.y
    };
    temp->direction = head->last->direction;
    
    head->last->next = temp;
    head->last = temp;
}


void add_body(body *head, body *bodies, int *numBodies)
{
    *numBodies += 1;
    init_body(head, bodies + *numBodies - 1);
}


void changeDirection(body *head, int changeX, int changeY)
{
    head->direction.x = changeX;
    head->direction.y = changeY;
}


void changePosition(body *head, body *bodies, int numBodies)
{
    head->position.x += head->direction.x;
    head->position.y += head->direction.y;
    
    for (int i = 0; i < numBodies; i++)
    {
        (bodies+i)->position.x += (bodies+i)->direction.x;
        (bodies+i)->position.y += (bodies+i)->direction.y;
    }
    
    for (int i = numBodies-1; i >= 0; i--)
        (bodies+i)->direction = (bodies+i)->former->direction;
}


int getInput(int *cont, body *head)
{
    int change = 0;
    if ( keyhit() )
    {
        switch ( getkey() )
        {
            case 'w':
                changeDirection(head, 0, -1);
                change = 1;
                break;
            case 's':
                changeDirection(head, 0, 1);
                change = 1;
                break;
            case 'a':
                changeDirection(head, -1, 0);
                change = 1;
                break;
            case 'd':
                changeDirection(head, 1, 0);
                change = 1;
                break;
            case 'q':
                clearscreen();
                printf("Exited\n");
                *cont = 0;
                break;
            default:
                break;
        }
    }
    msleep(FPS);
    return change;
}


void display(body head, vector2 fruit, int score, body *bodies, int numBodies)
{
    int out;
    for (int row = 0; row < HEIGHT; row++)
    {
        for (int col = 0; col < WIDTH; col++)
        {
            out = 0;
            
            if (
                row-1 == head.position.y &&
                col-1 == head.position.x
                )
            {
                putchar(SNAKE_SPRITE);
                continue;
            }
            
            for (int i = 0; i < numBodies; i++)
            {
                if (
                    row-1 == (bodies+i)->position.y &&
                    col-1 == (bodies+i)->position.x
                    )
                {
                    putchar(SNAKE_SPRITE);
                    out = 1;
                    break;
                }
            }
            
            if (out)
                continue;
                
            if (
                row-1 == fruit.y &&
                col-1 == fruit.x
                )
                putchar(FRUIT_SPRITE);
            else if (
                (row == 0 && col < WIDTH-1) ||
                (row == HEIGHT - 1 && col < WIDTH-1) ||
                col == 0 ||
                col == WIDTH - 2
                )
                putchar(BORDER_SPRITE);
            else if (
                col == WIDTH-1
                )
                putchar('\n');
            else
                putchar(' ');
        }
    }
    printf("Score: %d\n", score);
}


int main()
{
    srand(time(NULL));
    
    body head;
    head.former = NULL;
    head.next = NULL;
    head.last = &head;
    head.position = (vector2) { WIDTH/2-4, HEIGHT/2-2};
    head.direction = (vector2) {0, 0};
    
    body* bodies = calloc( (WIDTH-2) * (HEIGHT-2), sizeof(body) );
    int numBodies = 0;
    
    vector2 fruit = { rand() % (WIDTH-3), rand() % (HEIGHT-3) };
    int score = 0;
    
    int cont = 1;
    while(cont)
    {
        clearscreen();
        display(head, fruit, score, bodies, numBodies);
        getInput(&cont, &head);
        changePosition(&head, bodies, numBodies);
        
        
        if (
            head.position.x == fruit.x && 
            head.position.y == fruit.y
            )
        {
            score++;
            fruit.x = rand() % (WIDTH-3);
            fruit.y = rand() % (HEIGHT-3);
            add_body(&head, bodies, &numBodies);
        }
    }
    printf("done");
    
    free(bodies);
    bodies = NULL;
    
    return 0;
}
