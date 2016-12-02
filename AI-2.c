#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>

#include "game.h"

// how many simulations to run each computer move:
#define ROUNDS 3000    // lower == dumber

typedef enum winner{
	MARK_HUMAN,
	MARK_AI,
	MARK_TIE,
	MARK_NONE  //Game not Done
} Winner;

typedef enum currentPlayer{
    Human,
    AI
}CurrentPlayer;

/**
 * Check for a win on a particular board
 */
Winner checkWin(struct game *myGame)
{
  if( (myGame->scores[1]) > (myGame->scores[0]) && myGame->tiles_used == TILE_COUNT)  //scores[0] is AI
    return MARK_HUMAN;
  else if((myGame->scores[1]) < (myGame->scores[0]) && myGame->tiles_used == TILE_COUNT)
    return MARK_AI;
  else if((myGame->scores[1]) == (myGame->scores[0]) && myGame->tiles_used == TILE_COUNT)
    return MARK_TIE;
  else if(myGame->tiles_used != TILE_COUNT)
    return MARK_NONE;
}

/**
 * Copy a Game
 */
void copyGame(struct game *scrGame,struct game *destGame)
{
    *destGame = *scrGame;
  {/*
    for(int i = 0; i < AXIS*AXIS; i++){
      destGame->board.tiles[i] = scrGame->board.tiles[i];
      destGame->board.slot_spots[i] = scrGame->board.slot_spots[i];
    }

	destGame->board.empty_slot_count = scrGame->board.empty_slot_count;

	for(int i = 0; i < AXIS; i++){
      destGame->board.column_terminators[i] = scrGame->board.column_terminators[i];
	}

	for(int i = 0; i < TILE_COUNT; i++){
        for(int j = 0; j < 5; j++){
          destGame->tile_deck[i].edges[j] = scrGame->tile_deck[i].edges[j];
        }
        destGame->tile_deck[i].attribute = scrGame->tile_deck[i].attribute;
    }

    destGame->tiles_used = scrGame->tiles_used;


    for(int i = 0; i < PLAYER_COUNT; i++){
        destGame->scores[i] = scrGame->scores[i];
        destGame->meeples[i] = scrGame->meeples[i];
    }

      if(scrGame->features[0] != NULL){
        for(int i = 0; i < (AXIS * AXIS * 4 * 3); i++){
          (*(destGame->features[i])).weighted_size = (*(scrGame->features[i])).weighted_size;
          for(int j = 0; j < PLAYER_COUNT; i++){
            (*(destGame->features[i])).tigers[j] = (*(scrGame->features[i])).tigers[j];
            (*(destGame->features[i])).crocodiles[j] = (*(scrGame->features[i])).crocodiles[j];
          }
          for(int j = 0; j < 100; j++){
            (*(destGame->features[i])).neighbors[j] = (*(scrGame->features[i])).neighbors[j];
          }

          (*(destGame->features[i])).neighbor_count = (*(scrGame->features[i])).neighbor_count;

          for(int i = 0; i < 100; i++){
           (*(destGame->features[i])).open_slots[i].x = (*(scrGame->features[i])).open_slots[i].x;
           (*(destGame->features[i])).open_slots[i].y = (*(scrGame->features[i])).open_slots[i].y;
          }
          (*(destGame->features[i])).slot_count = (*(scrGame->features[i])).slot_count;
          (*(destGame->features[i])).type = (*(scrGame->features[i])).type;
        }
      }
      else{
        for(int i = 0; i < (AXIS*AXIS * 4 * 3); i++){
            destGame->features[i] = NULL;
        }
      }

    destGame->features_used = scrGame->features_used;*/}

}

/**int
 * Choose a random move on the board
 */
struct move randomMove(struct game *myGame, struct tile currentTile, CurrentPlayer player)
{
    //enum game_erro.r_code invalid_move(struct board b, struct move m, struct slot **adjs)  adjs is array of pointer  OK, E_TILE_EDGE_CONFLICT, E_TILE_NOT_PLACEABLE
	struct move possibleMove[13 * 4 * 2 * myGame->board.empty_slot_count];
	int moveCount = 0;
	int size = myGame->board.empty_slot_count;
	int currentPlayer = (player == AI)? 0: 1;
	for(int i = 0; i < size; i++){
      //random move for have no meeple
      if(myGame->meeples[currentPlayer] == 0){
        struct move m1 = {currentTile, myGame->board.slot_spots[i], 0, -1, -1};
	    if(is_move_valid(myGame, m1, currentPlayer) == OK){
          possibleMove[moveCount++] = m1;
	    }
        struct move m2 = {currentTile, myGame->board.slot_spots[i], 1, -1, -1};
	    if(is_move_valid(myGame, m2, currentPlayer) == OK){
          possibleMove[moveCount++] = m2;
	    }
        struct move m3 = {currentTile, myGame->board.slot_spots[i], 2, -1, -1};
	    if(is_move_valid(myGame, m3, currentPlayer) == OK){
          possibleMove[moveCount++] = m3;
	    }
        struct move m4 = {currentTile, myGame->board.slot_spots[i], 3, -1, -1};
	    if(is_move_valid(myGame, m4, currentPlayer) == OK){
          possibleMove[moveCount++] = m4;
	    }
      }
      else{
        //Have meeple random move
        for(int j = -1; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 0, j, -1};
	      //printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
         //   printf("m1 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }

        for(int j = -1; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 1, j, -1};
	     // printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
        //                    printf("m2 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }

        for(int j = -1; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 2, j, -1};
	      //printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
         //                   printf("m3 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }
        for(int j = -1; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 3, j, -1};
	    //  printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
          //                  printf("m4 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }        calculate_scores(myGame);
        //For Gator
        for(int j = 0; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 0, -1, j};
	    //  printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
          //                  printf("gator m1 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }
        for(int j = 0; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 1, -1, j};
	    //  printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
         //                   printf("gator m2 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }

        for(int j = 0; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 2, -1, j};
	    //  printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
       //                     printf("gator m3 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }
        for(int j = 0; j < 13; j++){
	      struct move m = {currentTile, myGame->board.slot_spots[i], 3, -1, j};
	   //   printf("result of valid move: %i, when tiger: %i, gator: %i\n",is_move_valid(myGame, m, 0), m.tcorner, m.ccorner);
	      if(is_move_valid(myGame, m, currentPlayer) == OK){
        //                    printf("gator m4 OK == 0\n");
            possibleMove[moveCount++] = m;
	      }
        }
      }
	}

	/*printf("%i\n", moveCount);
	for(int i = 0; i < moveCount; i++){
        printf("slot X: %i, Y: %i, rotation: %i, tiger: %i, gator: %i\n", possibleMove[i].slot.x, possibleMove[i].slot.y,  possibleMove[i].rotation, possibleMove[i].tcorner, possibleMove[i].ccorner);
    }*/

    srand (time(NULL));
	return possibleMove[rand()%moveCount];
}

/** Run random games from a starting position
 *
 * @param board the starting board configuration
 * @param currentrealplayer whose turn it is next in the real world
 * @param count the number of simulations to run
 *
 * @return the board position that has the best score, or -1 if there's no place to go
 */
 //AI know its tile stack
 //This function will give best move for AI
 //If the game is over, don't call this function
 //If AI run out of tile, don't call this function
struct move runSimulations(struct game *myGame, struct tile *AITileStack, struct tile *HumanTileStack, int count){
	struct move pos;
	struct move firstMove;
    int winCount[count];
    struct move winMove[count];
	int maxWins;

	struct game *tempGame = malloc(sizeof(*tempGame));
    CurrentPlayer player;
    Winner winner;

    /**if(myGame->tiles_used == (2 * TILE_COUNT)) return emptyMOVE**/

    //combine AI tile stack with Human tile stack as one tile stack (human first -- AI first -> human second -- AI second ...)
    /**struct tile orderedTile[2 * TILE_COUNT];**/
    struct tile orderedTile[2 * 5];
    /**for(int i = 0; i < 2*TILE_COUNT; i++){**/
    for(int i = 0; i < 2*5; i++){
      orderedTile[i] = (i%2 == 0)? HumanTileStack[i/2]: AITileStack[i/2];
    }

	for(int i = 0; i < count; i++){
      copyGame(myGame,tempGame);
      player = AI;
      firstMove.rotation = INT_MIN;

      /** for(int j = tempGame.tiles_used; j < 2*TILE_COUNT; i++){**/
      for(int j = tempGame->tiles_used; j < 10; j++){
        printf("run simulation j:  %i\n", j);
        pos = randomMove(tempGame, orderedTile[j], player);

        /**play_move(tempGame, pos, player);**/  //play_more function need to be fix

        calculate_scores(tempGame);
        winner = checkWin(tempGame);
        if (winner != MARK_NONE) break;

        if (firstMove.rotation == INT_MIN) firstMove = pos;
        player = (player == Human)? AI : Human;
      }

       winCount[i] = tempGame->scores[0] - tempGame->scores[1];  // AI's score - Human's score
       winMove[i] = firstMove;
	}

	maxWins = INT_MIN;
	for(int i = 0; i < count; i++){
      if(winCount[i] > maxWins){
        maxWins = winCount[i];
        firstMove = winMove[i];
      }
	}
	return firstMove;
}

int main(void)
{
    struct game *games = malloc(sizeof(*games));
    make_game(games);
    enum edge edges[5];
    edges[0] = CITY;     //1
    edges[1] = FIELD;      //1
    edges[2] = FIELD;     //2
    edges[3] = FIELD;      //3
    edges[4] = FIELD;      //1

    enum attribute attributes = SHIELD;  //1
    struct tile setTile = make_tile(edges, attributes);

{
    /*struct slot setSlot;;
    setSlot.x = 71;
    setSlot.y = 71;
    //int play_move(struct game *g, struct move m, int player);
    //struct move make_move(struct tile t, struct slot s, int rotation,int tcorner, int ccorner)
    struct move setMove = make_move(setTile, setSlot, 0, 0, 0);
    printf("here\n");
    int a = play_move(games, setMove, 0);
        printf("here\n");
    printf("First play move: %i\n", a);

    edges[0] = FIELD;     //1
    edges[1] = FIELD;      //1
    edges[2] = CITY;     //2
    edges[3] = FIELD;      //3
    edges[4] = FIELD;      //1
    setTile = make_tile(edges, attributes);
    setSlot.x = 71;
    setSlot.y = 71;
    setMove = make_move(setTile, setSlot, 0, 1, 1);
    printf("Second play move: %i\n", play_move(games, setMove, 0));
    setSlot.x = 71;
    setSlot.y = 71;
    setMove = make_move(setTile, setSlot, 0, 1, 1);
    printf("Third play move: %i\n", play_move(games, setMove, 0));

    printf("computer game.scores[0]: %i\n", games->scores[0]);
    printf("Human game.scores[1]: %i\n", games->scores[1]);
    printf("Test randomMove function\n");*/}
    //struct move possibleRandomMove = randomMove(games, setTile, AI);
   // printf("random move slot X: %i, Y: %i, rotation: %i, tiger: %i, gator: %i\n", possibleRandomMove.slot.x, possibleRandomMove.slot.y,  possibleRandomMove.rotation, possibleRandomMove.tcorner, possibleRandomMove.ccorner);
    struct tile AItileStack[5];
    for(int i = 0; i< 5; i++){
      AItileStack[0].edges[i] = setTile.edges[i];
      AItileStack[1].edges[i] = setTile.edges[i];
      AItileStack[2].edges[i] = setTile.edges[i];
      AItileStack[3].edges[i] = setTile.edges[i];
      AItileStack[4].edges[i] = setTile.edges[i];
      AItileStack[i].attribute = setTile.attribute;
    }


    struct tile PlayertileStack[5];
    for(int i = 0; i< 5; i++){
      PlayertileStack[0].edges[i] = setTile.edges[i];
      PlayertileStack[1].edges[i] = setTile.edges[i];
      PlayertileStack[2].edges[i] = setTile.edges[i];
      PlayertileStack[3].edges[i] = setTile.edges[i];
      PlayertileStack[4].edges[i] = setTile.edges[i];
      PlayertileStack[i].attribute = setTile.attribute;
    }


    struct tile a = setTile;
    for(int i = 0; i < 5; i++){
        printf("edge: %i\n", a.edges[i]);
    }
    printf("attribute %i\n", a.attribute);
    //print possible slot
    struct board* b = &games->board;
    for (unsigned i = 0; i < b->empty_slot_count; ++i)
    {
        struct slot* s = &b->slot_spots[i];
        printf("x:%i, y:%i\n", s->x, s->y);
    }

    //runSimulation
    printf("Test runSimulation\n");
   struct move bestMove = runSimulations(games, AItileStack, PlayertileStack, 2);
     printf("best move slot X: %i, Y: %i, rotation: %i, tiger: %i, gator: %i\n", bestMove.slot.x, bestMove.slot.y,  bestMove.rotation, bestMove.tcorner, bestMove.ccorner);
	return 0;
}
