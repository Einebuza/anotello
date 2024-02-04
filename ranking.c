#include "../prot/rank.h"

rank *rank_init(void){
    
    rank *new_rank = malloc(sizeof(rank));
    new_rank->rank_name = malloc(16*sizeof(char));

    new_rank->rank_score = -1;
    memset(new_rank->rank_name,0,16);

    return new_rank;
}

void update_and_display_leaderboard(SDL_Renderer *renderer, char * new_name, int *score){

    FILE *best_scores = NULL;
    best_scores = fopen("best_scores.txt","r");

    // Si le fichier n'existe pas encore
    if (best_scores == NULL){
        best_scores = fopen("best_scores.txt","w");
        fprintf(best_scores, "%d %s",score, new_name);
        return;
    }

    // On initialise un tableau de rangs qui représente le classement

    rank *current_ranking[11];  // Ici on choisit de prendre un tableau de taille 11 afin de pouvoir gérer le cas où le tableau de 10 serait déjà rempli

    for (int i = 0; i<11; i++){
        current_ranking[i] = rank_init();
    }

    // Initialisation des variables (x et y sont coordonnées utilisées pour afficher le classement en utilisant SDL)
    int x = 400, y = 200, current_rank = 0, new_rank = 0;
    int scanned_score;
    
    char *scanned_name = malloc(17*sizeof(char));
    char *str_score = malloc((17+7+1)*sizeof(char));

    memset(str_score,0,strlen(str_score));
    memset(scanned_name,0,strlen(scanned_name));

    if (best_scores != NULL){
        while (fscanf(best_scores,"%d",&scanned_score) != EOF && current_rank < 9){

            fgets(scanned_name, 16 ,best_scores);  // La fonction fgets prend au maximum 16 caractères mais s'arrête au retour à la ligne s'il y en a moins de 16
            scanned_name[strlen(scanned_name)-1] = '\0';    // On supprime le retour à la ligne
            sprintf(str_score, "%d", scanned_score);    // On convertit le score scanné en caractères qu'on stock dans la variable str_score 
            strcat(str_score, scanned_name);    // Concaténation du nom à la chaîne contenant le score

            //On affecte les données lues dans le fichier au current_rank-ème rang du classement 
            current_ranking[current_rank]->rank_score = scanned_score;
            strncpy(current_ranking[current_rank]->rank_name, scanned_name, 16);
            
            //On avance de rang dans le classement 
            current_rank++;

            if (*score < scanned_score){
                new_rank++; 
            }
        }
        fclose(best_scores);
    }

    /*
    
    A la fin du parcours de la boucle while : 

        - current_rank prend une valeur qui est égale au nombre de couples scores/nom dans le classement
        - new_rank prend la valeur du rang auquel doit être inséré le nouveau score 
    
    */

    // On décale les scores inférieurs au nouveau score
    for (int i = current_rank; i>new_rank; i--){              
        strncpy(current_ranking[i]->rank_name, current_ranking[i-1]->rank_name, 16);
        current_ranking[i]->rank_score = current_ranking[i-1]->rank_score;
    }

    // Si le classement était déjà rempli : il faut retirer le score le plus bas du classement
    if (current_ranking[10]->rank_score != -1){
        free(current_ranking[10]->rank_name);
        free(current_ranking[10]);
        current_ranking[10] = rank_init();
        current_rank = 9;
    }

    // On ajoute le nouveau score dans le classement
    current_ranking[new_rank]->rank_score = *score;          
    strncpy(current_ranking[new_rank]->rank_name, new_name, 16);


    // On stock le nouveau classement dans un fichier texte et on l'affiche
    best_scores = fopen("best_scores.txt","w");


    // Affichage des noms/scores
    int c = 0;
    while (c <= current_rank){
        
        memset(str_score,0,strlen(str_score));

        sprintf(str_score,"%d" ,current_ranking[c]->rank_score);
        char *current_name = strdup(current_ranking[c]->rank_name);
        strcat(str_score, current_name);

        SDL_Add_Text(renderer, 255, 255, 255, 30, x, y+40*c, str_score);

        fprintf(best_scores,"%s\n",str_score);

        c++;
    }


    fclose(best_scores);

    SDL_Add_Text(renderer, 255, 255, 255, 70, 300, 100, "Leaderboard");
    SDL_Add_Text(renderer, 255, 255, 255, 30, 300, 700, "Press esc to return to main menu");
    SDL_RenderPresent(renderer);


    // On libère la mémoire utilisée    
    for (int i = 0; i < 11; i++){
        free(current_ranking[i]->rank_name);
        free(current_ranking[i]);
    }

    free(scanned_name);
    free(str_score);

}