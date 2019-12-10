#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  */
struct stats{
    int max;
    char *player_name;
    float avg;
    float diff;
};

typedef struct player{
    char name[20];
    int goals;
    struct player* next;
}player_t;

typedef struct group{
    char name[20];
    struct player* next_player;
    struct group* next;
    struct stats stat;
}team_t;

/*
 * Expected input
 * 	For players:
 * 		x. name: goals
 * 
 * 	For teams:
 * 		team: name1, name2 ... , namen
 */

player_t *create_player(char *name, int goals){
    /* Allocating memory for a new player */
    player_t *new_player = malloc(sizeof(player_t));
    /* Assigning values with given arguments 
     * You need to use strcpy() to copy into the array.*/
    strlcpy(new_player->name, name, sizeof(new_player->name));
    new_player->goals = goals;
    
    return new_player;
}

void create_player_link(player_t **head, player_t *new_node){
    new_node->next = *head;
    *head = new_node;
}

/* Mostly for quick debug */
void print_players(player_t *head){
    player_t *seeker = head;
    while(seeker != NULL){
        printf("| %-8s | %3d goals |\n", seeker->name, seeker->goals);
        seeker = seeker->next;
    }
}

player_t *get_player_w_name(player_t *head, char* name){
    player_t *seeker = head;
    while(seeker != NULL){
        if (strcmp(seeker->name, name) == 0) {
            return create_player(seeker->name,seeker->goals);
        }
        seeker = seeker->next;
    }
    return NULL;
}

team_t *create_team(char *name, char *players, player_t *p_head){
    team_t *new_team = malloc(sizeof(team_t));
    player_t *tmp_pl_head = NULL;
    player_t *current_player = malloc(sizeof(player_t));
    /* Set the name of the team. */
    strlcpy(new_team->name, name, sizeof(name));
    char p_name[sizeof(new_team->next_player->name)];
    int flag = 1;       // temp solution
    /* Set the players of the team */
    while(flag){
        if(strchr(players, ',') != NULL){
            sscanf(players, "%[^,]," , p_name);
            players += strlen(p_name) + 2;      // p_name + ", "
        }
        else{
	    /* last name */
            sscanf(players, "%s", p_name);
            flag = 0;
        }
        /* To a func?(rec) */
        current_player = get_player_w_name(p_head, p_name);
        current_player->next = tmp_pl_head;
        tmp_pl_head = current_player;
    }
    /* Setting team's starting player pointer */
    new_team->next_player = current_player;
    		
    return new_team;
}

void create_team_link(team_t **head, team_t *new_node){
    new_node->next = *head;
    *head = new_node;
}

void get_max_goals(team_t *head){
    player_t *seeker = malloc(sizeof(player_t));
    seeker = head->next_player;
    int max = 0;
    while(seeker != NULL){
        if(seeker->goals > max){
            head->stat.max = seeker->goals;
            head->stat.player_name = seeker->name;
            max = seeker->goals;
        }
        seeker = seeker->next;
    }
    free(seeker);
}

float get_average_goals(team_t *t_head){
        player_t *player = malloc(sizeof(player_t));
        player = t_head->next_player;
        int num_of_players = 0;
        int sum_goals = 0;
        float result = 0;
    
    while(player != NULL){
        sum_goals += player->goals; 
        player = player->next;
        num_of_players++;
    }
    /* x/0 is bad! */
    if(num_of_players != 0){
        result = sum_goals / (float)num_of_players;
    }
    return result;
}

void avg_max_dif(team_t *head){    
    head->stat.avg = get_average_goals(head);
    get_max_goals(head);
    head->stat.diff = head->stat.max - head->stat.avg;
}

void print_stats_for_all(team_t *head){
    team_t *seeker = malloc(sizeof(team_t));
    seeker = head;
    while(seeker != NULL){
        avg_max_dif(seeker);
        printf("Team: %s\nAverage goals: %5.2f\nBest player is %s with %d goals, which is %5.2f points away from the average of this team\n\n" ,
                seeker->name, seeker->stat.avg, 
                seeker->stat.player_name, seeker->stat.max, seeker->stat.diff);
        seeker = seeker->next;
    }
}

/* Terminate the program if file is missing */
void file_check(FILE* file, char *name){
    if(file == NULL){
        printf("File \"%s\" not found.\n", name);
        exit(EXIT_FAILURE);
    }
}

/*        MAIN FUNCTION        */
int main(int argc, char** argv) {
    
    /* Open the players file and check if it is available. */
	char players_file[] = "players.txt";
	char teams_file[] = "teams.txt";
    FILE *file = fopen(players_file, "r");
    file_check(file, players_file);
    /* list inits */
	player_t *p_head = NULL;
	player_t *p_tmp;
	team_t *t_head = NULL;
	team_t *t_tmp;
    /* var inits */
	char buffer[50];
	int line_length = sizeof(buffer);
	char name[sizeof(p_head->name)];
	char team_players[sizeof(buffer)];
	int goal;
    
    /* Processing players data */
    while(fgets(buffer, line_length, file)){
        /* Giving values from buffer with given pattern. 
         * "%*s" -> ignore
         * "%[^x]x" -> copy until x */
        sscanf(buffer, "%*s %[^:]: %d", name, &goal);
        /* creating a new player_t */
        p_tmp = create_player(name, goal);
        /* Then link it into the list */
        create_player_link(&p_head, p_tmp);
    }
    print_players(p_head);
    fclose(file);
    
    /* Processing team data */
    file = fopen(teams_file, "r");
    file_check(file, teams_file);;
    while(fgets(buffer, line_length, file)){       
	/* %[^:]: = name of team, %[^\n]\n -> player names */
        sscanf(buffer, "%[^:]: %[^\n]\n", name, team_players);
        t_tmp = create_team(name, team_players, p_head);
        create_team_link(&t_head, t_tmp);
    }
    /* t1: Calc stats for each team */
    print_stats_for_all(t_head);
     
    fclose(file);
    printf("File: %s\nDate: %s\nTime: %s\n", __FILE__, __DATE__, __TIME__);
    return (EXIT_SUCCESS);
}