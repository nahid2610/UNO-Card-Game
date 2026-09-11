#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 108


typedef struct {
    char color[10];
    char value[15];
} Card;


typedef struct CardNode {
    Card card;
    struct CardNode *next;
} CardNode;


typedef struct {
    char name[30];
    CardNode *hand;
    int cardCount;
    int score;
    int isComputer;
} Player;


typedef struct {
    Card cards[DECK_SIZE];
    int top;
} Stack;


Card deck[DECK_SIZE];
int deckSize = 0;

Stack drawPile;
Stack discardPile;

Player players[2];

int currentPlayer = 0;
int direction = 1;


void push(Stack *stack, Card card)
{
    if (stack->top < DECK_SIZE - 1)
    {
        stack->cards[++stack->top] = card;
    }
}

Card pop(Stack *stack)
{
    Card empty = {"", ""};

    if (stack->top >= 0)
    {
        return stack->cards[stack->top--];
    }

    return empty;
}


void addCard(char color[], char value[])
{
    if (deckSize < DECK_SIZE)
    {
        strcpy(deck[deckSize].color, color);
        strcpy(deck[deckSize].value, value);
        deckSize++;
    }
}


void createDeck()
{
    deckSize = 0;

    char *colors[] = {
        "Red",
        "Green",
        "Blue",
        "Yellow"
    };

    for (int c = 0; c < 4; c++)
    {
        addCard(colors[c], "0");

        for (int n = 1; n <= 9; n++)
        {
            char value[10];

            sprintf(value, "%d", n);

            addCard(colors[c], value);
            addCard(colors[c], value);
        }

        addCard(colors[c], "Skip");
        addCard(colors[c], "Skip");

        addCard(colors[c], "Reverse");
        addCard(colors[c], "Reverse");

        addCard(colors[c], "Draw2");
        addCard(colors[c], "Draw2");
    }

    for (int i = 0; i < 4; i++)
    {
        addCard("Wild", "Wild");
        addCard("Wild", "Wild4");
    }
}


void shuffleDeck()
{
    for (int i = deckSize - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}


void initializeStacks()
{
    drawPile.top = -1;
    discardPile.top = -1;

    for (int i = 0; i < deckSize; i++)
    {
        push(&drawPile, deck[i]);
    }
}


void addCardToHand(Player *player, Card card)
{
    CardNode *newNode =
        (CardNode *)malloc(sizeof(CardNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    newNode->card = card;
    newNode->next = NULL;

    if (player->hand == NULL)
    {
        player->hand = newNode;
    }
    else
    {
        CardNode *temp = player->hand;

        while (temp->next != NULL)
        {
            temp = temp->next;
        }

        temp->next = newNode;
    }

    player->cardCount++;
}


Card removeCardFromHand(Player *player, int index)
{
    Card empty = {"", ""};

    if (index < 1 || index > player->cardCount)
    {
        return empty;
    }

    CardNode *temp = player->hand;
    CardNode *previous = NULL;

    for (int i = 1; i < index; i++)
    {
        previous = temp;
        temp = temp->next;
    }

    Card selectedCard = temp->card;

    if (previous == NULL)
    {
        player->hand = temp->next;
    }
    else
    {
        previous->next = temp->next;
    }

    free(temp);

    player->cardCount--;

    return selectedCard;
}


void displayCard(Card card)
{
    printf("[%s - %s]", card.color, card.value);
}


void displayHumanHand(Player *player)
{
    CardNode *temp = player->hand;

    int index = 1;

    while (temp != NULL)
    {
        printf("%d. ", index);
        displayCard(temp->card);
        printf("\n");

        temp = temp->next;
        index++;
    }
}


int isValidCard(Card card, Card topCard)
{
    if (strcmp(card.color, topCard.color) == 0)
    {
        return 1;
    }

    if (strcmp(card.value, topCard.value) == 0)
    {
        return 1;
    }

    if (strcmp(card.color, "Wild") == 0)
    {
        return 1;
    }

    return 0;
}


void refillDrawPile()
{
    if (discardPile.top <= 0)
    {
        return;
    }

    Card topCard =
        discardPile.cards[discardPile.top];

    discardPile.top--;

    while (discardPile.top >= 0)
    {
        push(&drawPile, pop(&discardPile));
    }

    push(&discardPile, topCard);

    printf("\nDraw pile reshuffled!\n");
}


int drawOneCard(Player *player)
{
    if (drawPile.top == -1)
    {
        refillDrawPile();
    }

    if (drawPile.top >= 0)
    {
        Card card = pop(&drawPile);

        addCardToHand(player, card);

        return 1;
    }

    return 0;
}


void nextPlayer()
{
    currentPlayer += direction;

    if (currentPlayer >= 2)
    {
        currentPlayer = 0;
    }

    if (currentPlayer < 0)
    {
        currentPlayer = 1;
    }
}


void computerChooseColor(Card *card)
{
    int choice = rand() % 4;

    switch (choice)
    {
        case 0:
            strcpy(card->color, "Red");
            break;

        case 1:
            strcpy(card->color, "Green");
            break;

        case 2:
            strcpy(card->color, "Blue");
            break;

        case 3:
            strcpy(card->color, "Yellow");
            break;
    }
}


void humanChooseColor(Card *card)
{
    int choice;

    printf("\nChoose a color:\n");

    printf("1. Red\n");
    printf("2. Green\n");
    printf("3. Blue\n");
    printf("4. Yellow\n");

    printf("Enter choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            strcpy(card->color, "Red");
            break;

        case 2:
            strcpy(card->color, "Green");
            break;

        case 3:
            strcpy(card->color, "Blue");
            break;

        case 4:
            strcpy(card->color, "Yellow");
            break;

        default:
            strcpy(card->color, "Red");
            printf("Invalid choice. Red selected.\n");
    }
}


void applyEffect(Card card)
{
    if (strcmp(card.value, "Skip") == 0)
    {
        printf("\nNext player is skipped!\n");

        nextPlayer();
    }

    else if (strcmp(card.value, "Reverse") == 0)
    {
        direction *= -1;

        printf("\nDirection reversed!\n");

        nextPlayer();
    }

    else if (strcmp(card.value, "Draw2") == 0)
    {
        nextPlayer();

        printf("\n%s must draw 2 cards!\n",
               players[currentPlayer].name);

        drawOneCard(&players[currentPlayer]);
        drawOneCard(&players[currentPlayer]);
    }

    else if (strcmp(card.value, "Wild4") == 0)
    {
        nextPlayer();

        printf("\n%s must draw 4 cards!\n",
               players[currentPlayer].name);

        for (int i = 0; i < 4; i++)
        {
            drawOneCard(&players[currentPlayer]);
        }
    }
}


int findComputerCard(Player *computer, Card topCard)
{
    CardNode *temp = computer->hand;

    int index = 1;

    while (temp != NULL)
    {
        if (isValidCard(temp->card, topCard))
        {
            return index;
        }

        temp = temp->next;
        index++;
    }

    return -1;
}


int humanTurn()
{
    Player *player = &players[0];

    Card topCard =
        discardPile.cards[discardPile.top];

    printf("\n");
    printf("========================================\n");
    printf("              YOUR TURN\n");
    printf("========================================\n");

    printf("\nTop Card: ");
    displayCard(topCard);

    printf("\n\nYour Cards:\n");

    displayHumanHand(player);

    printf("\nCards remaining: %d\n",
           player->cardCount);

    int choice;

    printf("\nEnter card number to play");
    printf(" (0 = Draw): ");

    scanf("%d", &choice);

    if (choice == 0)
    {
        printf("\nYou draw a card.\n");

        if (drawOneCard(player))
        {
            CardNode *last = player->hand;

            while (last->next != NULL)
            {
                last = last->next;
            }

            printf("You drew: ");
            displayCard(last->card);
            printf("\n");

            if (isValidCard(last->card, topCard))
            {
                char answer;

                printf("\nYou can play the drawn card.");
                printf(" Play it? (y/n): ");

                scanf(" %c", &answer);

                if (answer == 'y' || answer == 'Y')
                {
                    choice = player->cardCount;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }

    if (choice < 1 ||
        choice > player->cardCount)
    {
        printf("\nInvalid card number!\n");
        return 0;
    }

    CardNode *temp = player->hand;

    for (int i = 1; i < choice; i++)
    {
        temp = temp->next;
    }

    Card selectedCard = temp->card;

    if (!isValidCard(selectedCard, topCard))
    {
        printf("\nInvalid move!\n");
        printf("Card must match color or value.\n");

        return 0;
    }

    selectedCard =
        removeCardFromHand(player, choice);

    printf("\nYou played: ");
    displayCard(selectedCard);
    printf("\n");

    if (strcmp(selectedCard.color, "Wild") == 0)
    {
        if (strcmp(selectedCard.value, "Wild") == 0)
        {
            humanChooseColor(&selectedCard);
        }
        else
        {
            humanChooseColor(&selectedCard);
        }

        printf("Selected color: %s\n",
               selectedCard.color);
    }

    push(&discardPile, selectedCard);

    if (player->cardCount == 1)
    {
        printf("\n*** UNO! ***\n");
    }

    if (player->cardCount == 0)
    {
        return 2;
    }

    if (strcmp(selectedCard.value, "Skip") == 0 ||
        strcmp(selectedCard.value, "Reverse") == 0 ||
        strcmp(selectedCard.value, "Draw2") == 0 ||
        strcmp(selectedCard.value, "Wild4") == 0)
    {
        applyEffect(selectedCard);

        return 3;
    }

    return 1;
}


int computerTurn()
{
    Player *computer = &players[1];

    Card topCard =
        discardPile.cards[discardPile.top];

    printf("\n");
    printf("========================================\n");
    printf("          COMPUTER'S TURN\n");
    printf("========================================\n");

    printf("\nTop Card: ");
    displayCard(topCard);

    printf("\nComputer has %d cards.\n",
           computer->cardCount);

    int choice =
        findComputerCard(computer, topCard);

    if (choice == -1)
    {
        printf("\nComputer has no valid card.\n");
        printf("Computer draws a card.\n");

        if (drawOneCard(computer))
        {
            CardNode *last = computer->hand;

            while (last->next != NULL)
            {
                last = last->next;
            }

            printf("Computer drew: ");
            displayCard(last->card);
            printf("\n");

            if (isValidCard(last->card, topCard))
            {
                choice = computer->cardCount;

                printf("Computer plays the drawn card.\n");
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }

    CardNode *temp = computer->hand;

    for (int i = 1; i < choice; i++)
    {
        temp = temp->next;
    }

    Card selectedCard = temp->card;

    selectedCard =
        removeCardFromHand(computer, choice);

    printf("\nComputer played: ");
    displayCard(selectedCard);
    printf("\n");

    if (strcmp(selectedCard.color, "Wild") == 0)
    {
        computerChooseColor(&selectedCard);

        printf("Computer selected color: %s\n",
               selectedCard.color);
    }

    push(&discardPile, selectedCard);

    if (computer->cardCount == 1)
    {
        printf("\n*** COMPUTER SAYS UNO! ***\n");
    }

    if (computer->cardCount == 0)
    {
        return 2;
    }

    if (strcmp(selectedCard.value, "Skip") == 0 ||
        strcmp(selectedCard.value, "Reverse") == 0 ||
        strcmp(selectedCard.value, "Draw2") == 0 ||
        strcmp(selectedCard.value, "Wild4") == 0)
    {
        applyEffect(selectedCard);

        return 3;
    }

    return 1;
}


void dealCards()
{
    for (int round = 0; round < 7; round++)
    {
        for (int p = 0; p < 2; p++)
        {
            Card card = pop(&drawPile);

            addCardToHand(&players[p], card);
        }
    }
}


void initializePlayers()
{
    players[0].hand = NULL;
    players[0].cardCount = 0;
    players[0].score = 0;
    players[0].isComputer = 0;

    strcpy(players[0].name, "You");

    players[1].hand = NULL;
    players[1].cardCount = 0;
    players[1].score = 0;
    players[1].isComputer = 1;

    strcpy(players[1].name, "Computer");
}


int calculateScore(Player *player)
{
    int score = 0;

    CardNode *temp = player->hand;

    while (temp != NULL)
    {
        Card card = temp->card;

        if (strcmp(card.value, "Skip") == 0 ||
            strcmp(card.value, "Reverse") == 0 ||
            strcmp(card.value, "Draw2") == 0)
        {
            score += 20;
        }

        else if (strcmp(card.value, "Wild") == 0 ||
                 strcmp(card.value, "Wild4") == 0)
        {
            score += 50;
        }

        else
        {
            score += atoi(card.value);
        }

        temp = temp->next;
    }

    return score;
}


void displayComputerStatus()
{
    printf("\nComputer has %d cards.\n",
           players[1].cardCount);
}


void freeHand(Player *player)
{
    CardNode *temp = player->hand;

    while (temp != NULL)
    {
        CardNode *next = temp->next;

        free(temp);

        temp = next;
    }

    player->hand = NULL;
}


void playGame()
{
    Card firstCard = pop(&drawPile);

    while (strcmp(firstCard.value, "Wild4") == 0)
    {
        push(&drawPile, firstCard);

        firstCard = pop(&drawPile);
    }

    push(&discardPile, firstCard);

    printf("\n");
    printf("========================================\n");
    printf("             GAME STARTED\n");
    printf("========================================\n");

    printf("\nFirst Card: ");
    displayCard(firstCard);
    printf("\n");

    currentPlayer = 0;

    while (1)
    {
        int result;

        if (currentPlayer == 0)
        {
            result = humanTurn();
        }
        else
        {
            result = computerTurn();
        }

        if (result == 2)
        {
            printf("\n");
            printf("========================================\n");
            printf("              GAME OVER\n");
            printf("========================================\n");

            if (currentPlayer == 0)
            {
                printf("\n*** YOU WIN! ***\n");
            }
            else
            {
                printf("\n*** COMPUTER WINS! ***\n");
            }

            break;
        }

        if (result == 0)
        {
            continue;
        }

        nextPlayer();
    }
}


int main()
{
    srand(time(NULL));

    printf("\n");
    printf("============================================\n");
    printf("          UNO CARD GAME\n");
    printf("        HUMAN vs COMPUTER\n");
    printf("============================================\n");

    initializePlayers();

    createDeck();

    printf("\nCreating UNO deck...\n");

    shuffleDeck();

    printf("Shuffling deck...\n");

    initializeStacks();

    dealCards();

    printf("\nCards dealt successfully!\n");

    printf("\nYou have %d cards.\n",
           players[0].cardCount);

    printf("Computer has %d cards.\n",
           players[1].cardCount);

    printf("\nPress ENTER to start the game...");

    getchar();
    getchar();

    playGame();

    players[0].score =
        calculateScore(&players[0]);

    players[1].score =
        calculateScore(&players[1]);

    printf("\n");
    printf("========================================\n");
    printf("             FINAL SCORE\n");
    printf("========================================\n");

    printf("\nYour remaining cards: %d\n",
           players[0].cardCount);

    printf("Your score: %d\n",
           players[0].score);

    printf("\nComputer remaining cards: %d\n",
           players[1].cardCount);

    printf("Computer score: %d\n",
           players[1].score);

    freeHand(&players[0]);
    freeHand(&players[1]);

    printf("\nThank you for playing UNO!\n");

    return 0;
}
