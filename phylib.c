#include "phylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// part 1 constructor functions
phylib_object *phylib_new_still_ball( unsigned char number,
phylib_coord *pos ){
   phylib_object *obj = calloc(1,sizeof(phylib_object));
   // check if malloc worked
   if(obj==NULL){
       return NULL;
   }
   obj->type = PHYLIB_STILL_BALL;
   obj->obj.still_ball.number = number;
   obj->obj.still_ball.pos = *pos;

   return obj;
}

phylib_object *phylib_new_rolling_ball( unsigned char number,
phylib_coord *pos,
phylib_coord *vel,
phylib_coord *acc ){
   phylib_object *obj = calloc(1,sizeof(phylib_object));
   // check if malloc worked
   if(obj==NULL){
       return NULL;
   }
   obj->type = PHYLIB_ROLLING_BALL;
   obj->obj.rolling_ball.number = number;
   obj->obj.rolling_ball.pos = *pos;
   obj->obj.rolling_ball.vel = *vel;
   obj->obj.rolling_ball.acc = *acc;

   return obj;
}


phylib_object *phylib_new_hole( phylib_coord *pos ){
   phylib_object *obj = calloc(1,sizeof(phylib_object));
   // check if malloc worked
   if(obj==NULL){
       return NULL;
   }
   obj->type = PHYLIB_HOLE;
   obj->obj.hole.pos = *pos;


   return obj;
}


phylib_object *phylib_new_hcushion( double y ){
   phylib_object *obj = calloc(1,sizeof(phylib_object));
   // check if malloc worked
   if(obj==NULL){
       return NULL;
   }
   obj->type = PHYLIB_HCUSHION;
   obj->obj.hcushion.y = y;


   return obj;
}


phylib_object *phylib_new_vcushion( double x ){
   phylib_object *obj = calloc(1,sizeof(phylib_object));
   // check if malloc worked
   if(obj==NULL){
       return NULL;
   }
   obj->type = PHYLIB_VCUSHION;
   obj->obj.vcushion.x = x;


   return obj;
}


phylib_table *phylib_new_table( void ){
   phylib_table *table = calloc(1,sizeof(phylib_table));
   // check if malloc worked
   if(table==NULL){
       return NULL;
   }

   table->time = 0.0;
   table->object[0] = phylib_new_hcushion(0.0);
   table->object[1] = phylib_new_hcushion(PHYLIB_TABLE_LENGTH);
   table->object[2] = phylib_new_vcushion(0.0);
   table->object[3] = phylib_new_vcushion(PHYLIB_TABLE_WIDTH);


   table->object[4] = phylib_new_hole(&(phylib_coord){0.0, 0.0});
   table->object[5] = phylib_new_hole(&(phylib_coord){0.0, PHYLIB_TABLE_WIDTH});
   table->object[6] = phylib_new_hole(&(phylib_coord){0.0, PHYLIB_TABLE_LENGTH});
   table->object[7] = phylib_new_hole(&(phylib_coord){PHYLIB_TABLE_WIDTH, 0.0});
   table->object[8] = phylib_new_hole(&(phylib_coord){PHYLIB_TABLE_WIDTH, PHYLIB_TABLE_WIDTH});
   table->object[9] = phylib_new_hole(&(phylib_coord){PHYLIB_TABLE_WIDTH, PHYLIB_TABLE_LENGTH});


    // set any other objects to null
   for(int i=10;i<PHYLIB_MAX_OBJECTS;i++){
       table->object[i] = NULL;
   }

   return table;
}


//part 2 functions
void phylib_copy_object( phylib_object **dest, phylib_object **src ){

   if(*src==NULL){
       *dest = NULL;
       // both null, we are done
       return;
   }
  
   // else allocate memory
   *dest = (phylib_object *)calloc(1,sizeof(phylib_object));

   // check if malloc worked
   if(*dest==NULL){
       printf("Memory allocation failed.");
       return;
   }
   memcpy(*dest, *src, sizeof(phylib_object));
}


phylib_table *phylib_copy_table(phylib_table *table) {
    if (table == NULL) {
        return NULL;
    }

    // allocate memory for the new phylib_table structure
    phylib_table *tableCopy = (phylib_table *)calloc(1,sizeof(phylib_table));

    if (tableCopy == NULL) {
        printf("Memory allocation failed.");
        return NULL;
    }

    // copy the contents of the original table to the new memory location
    memcpy(tableCopy, table, sizeof(phylib_table));

    // copy each object in table
    for (int i = 0; i < PHYLIB_MAX_OBJECTS; i++) {
            phylib_copy_object(&(tableCopy->object[i]), &(table->object[i]));
        
    }

    return tableCopy;
}



void phylib_add_object( phylib_table *table, phylib_object *object ){
    // check if table is null
   if(table==NULL){
       return;
   }
   for(int i=0;i<PHYLIB_MAX_OBJECTS;i++){
       if(table->object[i]==NULL){
           table->object[i]=object;
           return;
       }
   }
}


void phylib_free_table(phylib_table *table) {
    // check if table is null
    if (table == NULL) {
        return;
    }
    // iterate through object array and free
    for (int i = 0; i < PHYLIB_MAX_OBJECTS; i++) {
        if (table->object[i] != NULL) {
            free(table->object[i]);
            table->object[i] = NULL;
        }
    }
    free(table);
}

phylib_coord phylib_sub( phylib_coord c1, phylib_coord c2 ){
   phylib_coord difference;

   // calculate difference for both coordinates
   difference.x = c1.x-c2.x;
   difference.y = c1.y-c2.y;

   return difference;
}


double phylib_length( phylib_coord c ){

    double result = sqrt(c.x*c.x+c.y*c.y);

   return result;
}


double phylib_dot_product( phylib_coord a, phylib_coord b ){
   double result;
   // get dot product
   result = a.x*b.x+a.y*b.y;


   return result;
}

double phylib_distance( phylib_object *obj1, phylib_object *obj2 ){
   double distance=0;
   // check for null
   if(obj1==NULL||obj2==NULL){
   //printf("objects are null");
    distance = -1.0;
   }

   if(obj1->type!=PHYLIB_ROLLING_BALL){
       // printf("object is not a ball");
       distance = -1.0;
   }

   else if(obj2->type==PHYLIB_STILL_BALL){
    //printf("\nstill ball");
       distance = phylib_length(phylib_sub(obj1->obj.rolling_ball.pos,obj2->obj.still_ball.pos)) - PHYLIB_BALL_DIAMETER;
   }

   else if(obj2->type==PHYLIB_ROLLING_BALL){
    //printf("\nrolling ball");
       distance = phylib_length(phylib_sub(obj1->obj.rolling_ball.pos,obj2->obj.rolling_ball.pos)) - PHYLIB_BALL_DIAMETER;
   }

   else if(obj2->type==PHYLIB_HOLE){
    //printf("\nhole");
       distance = phylib_length(phylib_sub(obj1->obj.rolling_ball.pos,obj2->obj.hole.pos)) - PHYLIB_HOLE_RADIUS;
   }

   else if(obj2->type==PHYLIB_HCUSHION){
    //printf("\nhcushion");
       distance = fabs(obj2->obj.hcushion.y - obj1->obj.rolling_ball.pos.y) - PHYLIB_BALL_RADIUS;
   }

   else if(obj2->type==PHYLIB_VCUSHION){
    //printf("\nvcushion");
       distance = fabs(obj2->obj.vcushion.x - obj1->obj.rolling_ball.pos.x) - PHYLIB_BALL_RADIUS;
   }
   else{
    //printf("something else occured");
       distance = -1.0;
   }
   return distance;
}


// part 3 functions
void phylib_roll( phylib_object *new, phylib_object *old, double time ){
    if(new==NULL||old==NULL){
        printf("Arguement is null.\n");
        return;
    }
    // check if arguements are rolling balls
   if(new->type!=PHYLIB_ROLLING_BALL || old->type!=PHYLIB_ROLLING_BALL){
       return;
    }

    // calculate positions and velocities
   new->obj.rolling_ball.pos.x = (old->obj.rolling_ball.pos.x + (old->obj.rolling_ball.vel.x*time) + (0.5*old->obj.rolling_ball.acc.x*time*time));


   new->obj.rolling_ball.pos.y = (old->obj.rolling_ball.pos.y + (old->obj.rolling_ball.vel.y*time) + (0.5*old->obj.rolling_ball.acc.y*time*time));


   new->obj.rolling_ball.vel.x = (old->obj.rolling_ball.vel.x+(old->obj.rolling_ball.acc.x*time));


   new->obj.rolling_ball.vel.y = (old->obj.rolling_ball.vel.y+(old->obj.rolling_ball.acc.y*time));

    // check if velocity sign changed
   if((new->obj.rolling_ball.vel.x * old->obj.rolling_ball.vel.x) < 0){
       new->obj.rolling_ball.vel.x = 0;
       new->obj.rolling_ball.acc.x = 0;
   }

   if((new->obj.rolling_ball.vel.y * old->obj.rolling_ball.vel.y) < 0){
       new->obj.rolling_ball.vel.y = 0;
       new->obj.rolling_ball.acc.y = 0;
   }
   
}

unsigned char phylib_stopped( phylib_object *object ){
    if(object==NULL){
        return 0;
    }

    if(phylib_length(object->obj.rolling_ball.vel)<PHYLIB_VEL_EPSILON){
        object->type = PHYLIB_STILL_BALL;

        phylib_coord pos = object->obj.rolling_ball.pos;

        object = phylib_new_still_ball(object->obj.rolling_ball.number, &pos);
        free(object);

        return 1;
    }
        return 0;
}

void phylib_bounce(phylib_object **a, phylib_object **b) {
    if (*a == NULL || *b == NULL) {
        printf("Null objects\n");
        return;
    }

    phylib_coord r_ab, v_rel, n;
    double r_ab_length, v_rel_n, a_speed, b_speed;

    switch ((*b)->type) {
        case PHYLIB_HCUSHION:
        //printf("hit hcushion");
            (*a)->obj.rolling_ball.vel.y = -(*a)->obj.rolling_ball.vel.y;
            (*a)->obj.rolling_ball.acc.y = -(*a)->obj.rolling_ball.acc.y;
            break;

        case PHYLIB_VCUSHION:
       // printf("hit vcushion");
            (*a)->obj.rolling_ball.vel.x = -(*a)->obj.rolling_ball.vel.x;
            (*a)->obj.rolling_ball.acc.x = -(*a)->obj.rolling_ball.acc.x;
            break;

        case PHYLIB_HOLE:
        //printf("hit hole");
            free(*a);
            *a = NULL; // set the pointer to NULL after freeing the memory
            break;

        case PHYLIB_STILL_BALL:
            // copy values from still ball to rolling ball
           // printf("hit still ball");
            (*b)->type = PHYLIB_ROLLING_BALL;
            (*b)->obj.rolling_ball.number = (*b)->obj.still_ball.number;
            (*b)->obj.rolling_ball.pos = (*b)->obj.still_ball.pos;
            (*b)->obj.rolling_ball.vel.x = 0.0;
            (*b)->obj.rolling_ball.vel.y = 0.0;
            (*b)->obj.rolling_ball.acc.x = 0.0;
            (*b)->obj.rolling_ball.acc.y = 0.0;
            // no break so the code will proceed to the next case

        case PHYLIB_ROLLING_BALL:
            // compute position of a with respect to b
          //  printf("hit rolling ball");
            r_ab = phylib_sub((*a)->obj.rolling_ball.pos, (*b)->obj.rolling_ball.pos);
            r_ab_length = phylib_length(r_ab);

            // avoid division by zero
            if (r_ab_length > 0.0) {
                // compute relative velocity of a with respect to b
                v_rel = phylib_sub((*a)->obj.rolling_ball.vel, (*b)->obj.rolling_ball.vel);

                // compute normal vector n
                n.x = r_ab.x / r_ab_length;
                n.y = r_ab.y / r_ab_length;

                // calculate ratio of v_rel by computing dot product of v_rel w respect to n
                v_rel_n = phylib_dot_product(v_rel, n);

                // update the velocity of ball a
                (*a)->obj.rolling_ball.vel.x -= v_rel_n * n.x;
                (*a)->obj.rolling_ball.vel.y -= v_rel_n * n.y;

                // update the velocity of ball b
                (*b)->obj.rolling_ball.vel.x += v_rel_n * n.x;
                (*b)->obj.rolling_ball.vel.y += v_rel_n * n.y;

                // compute speed of a and b as lengths of their velocities
                a_speed = phylib_length((*a)->obj.rolling_ball.vel);
                b_speed = phylib_length((*b)->obj.rolling_ball.vel);

                // if speed is greater than PHYLIB_VEL_EPSILON, set acceleration to neg vel divided by speed and multiplied by PHYLIB_DRAG
                if (a_speed > PHYLIB_VEL_EPSILON) {
                    (*a)->obj.rolling_ball.acc.x = -((*a)->obj.rolling_ball.vel.x / a_speed) * PHYLIB_DRAG;
                    (*a)->obj.rolling_ball.acc.y = -((*a)->obj.rolling_ball.vel.y / a_speed) * PHYLIB_DRAG;
                }

                if (b_speed > PHYLIB_VEL_EPSILON) {
                    (*b)->obj.rolling_ball.acc.x = -((*b)->obj.rolling_ball.vel.x / b_speed) * PHYLIB_DRAG;
                    (*b)->obj.rolling_ball.acc.y = -((*b)->obj.rolling_ball.vel.y / b_speed) * PHYLIB_DRAG;
                }
            }
            break;
    }
}

unsigned char phylib_rolling(phylib_table *t) {
    unsigned char count = 0;
    if (t == NULL) {
        return 0;
    }
    for (int i = 0; i < PHYLIB_MAX_OBJECTS; i++) {
        if (t->object[i] != NULL && t->object[i]->type == PHYLIB_ROLLING_BALL) {
            count++;
        }
    }
    return count;
}


phylib_table *phylib_segment(phylib_table *table) {
    // check if argument is null
    if (table == NULL) {
        printf("Table is null\n");
        return NULL;
    }

    // check if there are rolling balls
    if (phylib_rolling(table) == 0) {
        return NULL;
    }

    // copy the table
    phylib_table *newTable = phylib_copy_table(table);
    if (newTable == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // loop for time
    for (double time = PHYLIB_SIM_RATE; time <= PHYLIB_MAX_TIME; time += PHYLIB_SIM_RATE) {
        // loop through objects on the table
        for (int j = 0; j < PHYLIB_MAX_OBJECTS; j++) {
            if (newTable->object[j] != NULL && table->object[j] != NULL && newTable->object[j]->type == PHYLIB_ROLLING_BALL) {
                // apply phylib roll
                phylib_roll(newTable->object[j], table->object[j], time);

            }
        }

        // update time inside the loop
        newTable->time = time + table->time;

        for (int j = 0; j < PHYLIB_MAX_OBJECTS; j++) {
            // check if the ball has stopped
            if (newTable->object[j] != NULL && newTable->object[j]->type == PHYLIB_ROLLING_BALL) {
                if (phylib_stopped(newTable->object[j]) == 1) {
                    return newTable;
                }
            }
            // check for collision
            for (int k = 0; k < PHYLIB_MAX_OBJECTS; k++) {
                if (k != j && newTable->object[k] != NULL && newTable->object[j] != NULL && newTable->object[j]->type == PHYLIB_ROLLING_BALL) {
                    double distance = phylib_distance(newTable->object[j], newTable->object[k]);
                    // apply bounce if there is collision
                    if (distance < 0.0) {
                        phylib_bounce(&(newTable->object[j]), &(newTable->object[k]));
                        return newTable;
                    }
                }
            }
        }
        
    }
    // return table copy
    return newTable; 

}

char *phylib_object_string( phylib_object *object ){
    static char string[80];
    if (object==NULL){
        snprintf( string, 80, "NULL;" );
        return string;
    }
    switch (object->type){
        case PHYLIB_STILL_BALL:
            snprintf( string, 80,
            "STILL_BALL (%d,%6.1lf,%6.1lf)",
            object->obj.still_ball.number,
            object->obj.still_ball.pos.x,
            object->obj.still_ball.pos.y );
            break;
        case PHYLIB_ROLLING_BALL:
            snprintf( string, 80,
            "ROLLING_BALL (%d,%6.1lf,%6.1lf,%6.1lf,%6.1lf,%6.1lf,%6.1lf)",
            object->obj.rolling_ball.number,
            object->obj.rolling_ball.pos.x,
            object->obj.rolling_ball.pos.y,
            object->obj.rolling_ball.vel.x,
            object->obj.rolling_ball.vel.y,
            object->obj.rolling_ball.acc.x,
            object->obj.rolling_ball.acc.y );
            break;
        case PHYLIB_HOLE:
            snprintf( string, 80,
            "HOLE (%6.1lf,%6.1lf)",
            object->obj.hole.pos.x,
            object->obj.hole.pos.y );
        break;
        case PHYLIB_HCUSHION:
            snprintf( string, 80,
            "HCUSHION (%6.1lf)",
            object->obj.hcushion.y );
        break;
        case PHYLIB_VCUSHION:
            snprintf( string, 80,
            "VCUSHION (%6.1lf)",
            object->obj.vcushion.x );
        break;
    }
return string;
}
