/*pipebid.c*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXBUFF 65  // numero de caract. do buffer

char geraJogo[32] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6'};

int insere = 0, row = 0, column = 0;
int numeros[32] = {0}, numero_gerado, numeros_gerados = 0;

int playerUm = 0;
int playerDois = 0;
int salvaContexto = 0;

pthread_mutex_t mutex;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* verificaVencedor(void* arg) {
    pthread_mutex_lock(&mutex); // Bloqueia o mutex antes de acessar as variáveis compartilhadas

    if (playerDois > playerUm) {
        printf("\nVencedor: Player Dois -> %d\n\nDerrotado: Player Um -> %d\n", playerDois, playerUm);
    } else if(playerDois < playerUm){
        printf("\nVencedor: Player Um -> %d\n\nDerrotado: Player Dois -> %d\n", playerUm, playerDois);
    } else {
        printf("\nEmpate!\n\nPlayer Um -> %d\nPlayer Dois -> %d\n", playerUm, playerDois);
    }

    pthread_mutex_unlock(&mutex); // Libera o mutex após o término da operação
    sleep(1);
    return NULL;
}

void main()
{
   int	descritor,  // usado para criar o processo filho pelo fork
	pipe1[2],  // comunicacao pai -> filho
	pipe2[2];  // comunicacao filho -> pai

	pthread_t thread;

	pthread_mutex_init(&mutex, NULL);

   if (pipe(pipe1)<0 || pipe(pipe2) <0)
	{ printf("Erro na chamada PIPE");
	   exit(0);
	}

   //   Fork para criar o processo filho

    if ( (descritor = fork()) <0)
	{ printf("Erro na chamada FORK");
	   exit(0);
	}

	else if (descritor >0)  // PROCESSO PAI
	   {	close(pipe1[0]); // fecha leitura no pipe1
		close(pipe2[1]);  // fecha escrita no pipe2

		client(pipe2[0], pipe1[1]);  // Chama CLIENTE no PAI

		close(pipe1[1]); // fecha pipe1
		close(pipe2[0]);  // fecha pipe2
		//exit(0);

		wait(NULL);

		pthread_create(&thread, NULL, verificaVencedor, NULL);
        pthread_join(thread, NULL);

	    } // FIM DO PROCESSO PAI

	else // PROCESSO FILHO

	   {	close(pipe1[1]); // fecha escrita no pipe1
		close(pipe2[0]);  // fecha leitura no pipe2

		server(pipe1[0], pipe2[1]);  // Chama SERVIDOR no FILHO

		close(pipe1[0]); // fecha leitura no pipe1
		close(pipe2[1]);  // fecha escrita no pipe2
		exit(0);

	} // FIM DO PROCESSO FILHO

	pthread_mutex_destroy(&mutex);
	printf("\nFim de jogo\n\n");
} // FIM DO MAIN

/* -----------------------------------------------------------------------------------------------------------
Funcao Client: 	Executa no processo PAI
			Envia o nome do arquivo para o FILHO
			Recebe os dados do FILHO e imprime na tela
----------------------------------------------------------------------------------------------------------- */

client (readfd, writefd) //PAI
    int readfd, // leitura do pipe2[0]
    writefd; // escrita no pipe1[1]
{
    char buff[MAXBUFF];
    char posicaoUsuario[6];
    char enter;
    char transfereBuff[65] = {'\0'};

    int execPai = 1;
    int verificaPonto = 1;

    int veFinal = 1;

    int salvaContextoPlayerDois = 0;

	int i = 0, j = 0, flag = 1, trap = 1, k = 0;

    char cartas[8][8] = {
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
    };


	srand(time(NULL));

    while(numeros_gerados < 32){
        int numero_gerado = rand() % 32;
        if(numeros[numero_gerado] == 0){
            numeros[numero_gerado] = 1;
            while(i < 2) {
                int numero_gerado_dois = rand() % 8;
                int numero_gerado_tres = rand() % 8;
                if(cartas[numero_gerado_dois][numero_gerado_tres] == '0') {
                    cartas[numero_gerado_dois][numero_gerado_tres] = geraJogo[numero_gerado];
                    i++;
                }
            }
            i = 0;
            numeros_gerados++;
        }
    }

    /*char cartas[8][8] = {
        {'a', 'a', 'b', 'b', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
        {'9', '9', '9', '9', '9', '9', '9', '9'},
    };*/

    while(execPai)
    {
        printf("\033[2J\033[H");
        while(flag){
            printf("\tA\tB\tC\tD\tE\tF\tG\tH\tPAI\n\n\n");

            for(i = 0; i < 8; i++){
                for(j = 0; j < 8; j++){
                    if(cartas[i][j] == '9') {
                        salvaContextoPlayerDois++;
                        printf("\033[1;33m\t-\033[0m  ");
                    }
                    //if(cartas[i][j] == 0) printf("\t0  ", cartas[i][j]);
                    else printf("\t0  ", cartas[i][j]);
                }
                printf("\t-> %d\n\n", (i+1));
            }

            if(salvaContextoPlayerDois > salvaContexto) playerDois++;
            salvaContexto = salvaContextoPlayerDois;
            salvaContextoPlayerDois = 0;

            printf(" \n Digite a posição da 1ª e 2ª carta (Ex: A1 H7 ou . para o FIM): ");
            fgets(posicaoUsuario, 6, stdin);
            if(posicaoUsuario[0] == '.') {
                transfereBuff[64] = '0';
                execPai = 0;
                flag = 0;
                veFinal = 0;
                break;
            }
            if(posicaoUsuario[0] > 96 && posicaoUsuario[0] < 105) posicaoUsuario[0] -= 32;
            if(!(posicaoUsuario[0] > 64 && posicaoUsuario[0] < 73)) trap = 0;
            else {
                if(posicaoUsuario[1] < 49 || posicaoUsuario[1] > 56) trap = 0;
                else {
                    if(posicaoUsuario[2] != ' ') trap = 0;
                    else {
                        if(posicaoUsuario[3] > 96 && posicaoUsuario[3] < 105) posicaoUsuario[3] -= 32;
                        if(!(posicaoUsuario[3] > 64 && posicaoUsuario[3] < 73)) trap = 0;
                        if(posicaoUsuario[4] < 49 || posicaoUsuario[4] > 56) trap = 0;
                        if((posicaoUsuario[0] == posicaoUsuario[3]) && posicaoUsuario[1] == posicaoUsuario[4]) trap = 0;
                    }
                }
            }

            if(trap) {
                int pColunaUm = (posicaoUsuario[1] - '0') - 1;
                int pColunaDois = (posicaoUsuario[4] - '0') - 1;
                int pLinhaUm = posicaoUsuario[0] - 65;
                int pLinhaDois = posicaoUsuario[3] - 65;

                if(cartas[pColunaUm][pLinhaUm] == '9' || cartas[pColunaDois][pLinhaDois] == '9') trap = 0;
            }

            if(trap == 0) {
                printf("\033[2J\033[H");
                trap = 1;
            }
            else flag = 0;
        }

        int posColCartaUm = (posicaoUsuario[1] - '0') - 1;
        int posColCartaDois = (posicaoUsuario[4] - '0') - 1;

        int posLinCartaUm;
        int posLinCartaDois;

        posicaoUsuario[0] == 'A' ? posLinCartaUm = 0 : posicaoUsuario[0] == 'B' ? posLinCartaUm = 1 : posicaoUsuario[0] == 'C' ? posLinCartaUm = 2 : posicaoUsuario[0] == 'D' ? posLinCartaUm = 3 : posicaoUsuario[0] == 'E' ? posLinCartaUm = 4 : posicaoUsuario[0] == 'F' ? posLinCartaUm = 5 : posicaoUsuario[0] == 'G' ? posLinCartaUm = 6 : posicaoUsuario[0] == 'H' ? posLinCartaUm = 7 : posLinCartaUm;
        posicaoUsuario[3] == 'A' ? posLinCartaDois = 0 : posicaoUsuario[3] == 'B' ? posLinCartaDois = 1 : posicaoUsuario[3] == 'C' ? posLinCartaDois = 2 : posicaoUsuario[3] == 'D' ? posLinCartaDois = 3 : posicaoUsuario[3] == 'E' ? posLinCartaDois = 4 : posicaoUsuario[3] == 'F' ? posLinCartaDois = 5 : posicaoUsuario[3] == 'G' ? posLinCartaDois = 6 : posicaoUsuario[3] == 'H' ? posLinCartaDois = 7 : posLinCartaDois;

        printf("\033[2J\033[H");

        printf("\tA\tB\tC\tD\tE\tF\tG\tH\tPAI\n\n\n");

        for(i = 0; i < 8; i++){
            for(j = 0; j < 8; j++){
                if((i == posColCartaUm && j == posLinCartaUm) || (i == posColCartaDois && j == posLinCartaDois)) printf("\033[1;31m\t%c\033[0m  ", cartas[i][j]);
                //printf("%d",posColCartaUm);
                else if(cartas[i][j] != 0) printf("\t0  ", cartas[i][j]);
            }
            printf("\t-> %d\n\n", (i+1));
        }

        if(cartas[posColCartaUm][posLinCartaUm] == cartas[posColCartaDois][posLinCartaDois]){ //ACERTOU
            printf("\n\033[1;32m\tACERTOU\033[0m\n");
            pthread_mutex_lock(&mutex);
            playerUm++;
            salvaContexto+=2;
            pthread_mutex_unlock(&mutex);
            cartas[posColCartaUm][posLinCartaUm] = '9';
            cartas[posColCartaDois][posLinCartaDois] = '9';

            pthread_mutex_lock(&mutex);
            if(playerUm + playerDois > 31) verificaPonto = 0;
            pthread_mutex_unlock(&mutex);

        } else if(veFinal) printf("\n\033[1;31m\tERROU\033[0m\n");

        if(veFinal){
            char dummy_buffer[2];
            fgets(dummy_buffer, sizeof(dummy_buffer), stdin); // Ler e descartar a linha
            printf("\nPressione Enter para continuar...");
            fgets(dummy_buffer, sizeof(dummy_buffer), stdin); // Esperar pela próxima linha

                    flag = 1;
                    enter = '\0';
                    for(i = 0; i < 8; i++){
                        for(j = 0; j < 8; j++){
                            transfereBuff[k] = cartas[i][j];
                            k++;
                        }
                    }
                    if(verificaPonto) transfereBuff[64] = '1';
                    else transfereBuff[64] = '0';
                    k = 0;
                    strcpy(buff, transfereBuff);
                    write(writefd, buff, 65);

            read(readfd,buff,65);
        }
        //printf(" \n Client <- %s\n",buff);

        if(buff[64] == '0') {
            //printf("\n\nAqui");
            //getchar();
            execPai = 0;
            transfereBuff[64] = '0';
            break;
        } else transfereBuff[64] = '1';

        for(i = 0; i < 8; i++){
            for(j = 0; j < 8; j++){
                cartas[i][j] = buff[k];
                //buff[k] = '\0';
                k++;
            }
        }
        k = 0;
    }
} // Fim da Funcao CLIENT

/* -----------------------------------------------------------------------------------------------------------
Funcao Server: 		Executa no processo FILHO
			Abre o arquivo solicitado e envia seu conteudo
			para o PAI
----------------------------------------------------------------------------------------------------------- */
server(readfd, writefd) //FILHO
int readfd, // leitura do pipe1[0]
    writefd; // escrita no pipe2[1]

{
	char buff[MAXBUFF];
	int n, fd, i = 0, j = 0, k = 0, flag = 1, trap = 1;
	char transfereBuff[65] = {'\0'};
	char posicaoUsuario[6] = {'\0'};
	char enter;
	int execFilho = 1;

	int verificaPonto = 1;
	int veFinal = 1;

	char cartas[8][8] = { //Linha Coluna
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
        {'0', '0', '0', '0', '0', '0', '0', '0'},
    };

	while(execFilho){

        read(readfd,buff,65);

		printf("\033[2J\033[H");
		//printf(" \n Server<- %s",buff);

		if(buff[64] == '0') {
            //printf("\n\nAqui");
            execFilho = 0;
            transfereBuff[64] = '0';
            break;
        } else transfereBuff[64] = '1';

		for(i = 0; i < 8; i++){
                for(j = 0; j < 8; j++){
                    cartas[i][j] = buff[k];
                    //buff[k] = '\0';
                    k++;
                }
        }

        k = 0;

        printf("\033[2J\033[H");

        while(flag){
            printf("\n\tA\tB\tC\tD\tE\tF\tG\tH\tFILHO\n\n\n");

            for(i = 0; i < 8; i++){
                for(j = 0; j < 8; j++){
                    if(cartas[i][j] == '9') printf("\033[1;33m\t-\033[0m  ");
                    //if(cartas[i][j] == 0) printf("\t0  ", cartas[i][j]);
                    else printf("\t0  ", cartas[i][j]);
                }
                printf("\t-> %d\n\n", (i+1));
            }

            printf(" \n Digite a posição da 1ª e 2ª carta (Ex: A1 H7 ou . para o FIM): ");
            fgets(posicaoUsuario, 6, stdin);
            if(posicaoUsuario[0] == '.') {
                transfereBuff[64] = '0';
                veFinal = 0;
                break;
            }
            if(posicaoUsuario[0] > 96 && posicaoUsuario[0] < 105) posicaoUsuario[0] -= 32;
            if(!(posicaoUsuario[0] > 64 && posicaoUsuario[0] < 73)) trap = 0;
            else {
                if(posicaoUsuario[1] < 49 || posicaoUsuario[1] > 56) trap = 0;
                else {
                    if(posicaoUsuario[2] != ' ') trap = 0;
                    else {
                        if(posicaoUsuario[3] > 96 && posicaoUsuario[3] < 105) posicaoUsuario[3] -= 32;
                        if(!(posicaoUsuario[3] > 64 && posicaoUsuario[3] < 73)) trap = 0;
                        if(posicaoUsuario[4] < 49 || posicaoUsuario[4] > 56) trap = 0;
                        if((posicaoUsuario[0] == posicaoUsuario[3]) && posicaoUsuario[1] == posicaoUsuario[4]) trap = 0;
                    }
                }
            }

            if(trap) {
                int pColunaUm = (posicaoUsuario[1] - '0') - 1;
                int pColunaDois = (posicaoUsuario[4] - '0') - 1;
                int pLinhaUm = posicaoUsuario[0] - 65;
                int pLinhaDois = posicaoUsuario[3] - 65;

                if(cartas[pColunaUm][pLinhaUm] == '9' || cartas[pColunaDois][pLinhaDois] == '9') trap = 0;
            }

            if(trap == 0) {
                printf("\033[2J\033[H");
                trap = 1;
            }
            else flag = 0;
        }

		for(i = 0; i < 8; i++){
            for(j = 0; j < 8; j++){
                transfereBuff[k] = cartas[i][j];
                k++;
            }
        }
        k = 0;
        int posColCartaUm = (posicaoUsuario[1] - '0') - 1;
        int posColCartaDois = (posicaoUsuario[4] - '0') - 1;

        int posLinCartaUm;
        int posLinCartaDois;

        posicaoUsuario[0] == 'A' ? posLinCartaUm = 0 : posicaoUsuario[0] == 'B' ? posLinCartaUm = 1 : posicaoUsuario[0] == 'C' ? posLinCartaUm = 2 : posicaoUsuario[0] == 'D' ? posLinCartaUm = 3 : posicaoUsuario[0] == 'E' ? posLinCartaUm = 4 : posicaoUsuario[0] == 'F' ? posLinCartaUm = 5 : posicaoUsuario[0] == 'G' ? posLinCartaUm = 6 : posicaoUsuario[0] == 'H' ? posLinCartaUm = 7 : posLinCartaUm;
        posicaoUsuario[3] == 'A' ? posLinCartaDois = 0 : posicaoUsuario[3] == 'B' ? posLinCartaDois = 1 : posicaoUsuario[3] == 'C' ? posLinCartaDois = 2 : posicaoUsuario[3] == 'D' ? posLinCartaDois = 3 : posicaoUsuario[3] == 'E' ? posLinCartaDois = 4 : posicaoUsuario[3] == 'F' ? posLinCartaDois = 5 : posicaoUsuario[3] == 'G' ? posLinCartaDois = 6 : posicaoUsuario[3] == 'H' ? posLinCartaDois = 7 : posLinCartaDois;

        printf("\033[2J\033[H");

        printf("\tA\tB\tC\tD\tE\tF\tG\tH\tFILHO\n\n\n");

        for(i = 0; i < 8; i++){
            for(j = 0; j < 8; j++){
                if((i == posColCartaUm && j == posLinCartaUm) || (i == posColCartaDois && j == posLinCartaDois)) printf("\033[1;31m\t%c\033[0m  ", cartas[i][j]);
                //printf("%d",posColCartaUm);
                else if (cartas[i][j] != 0) printf("\t0  ", cartas[i][j]);
            }
            printf("\t-> %d\n\n", (i+1));
        }

        if(cartas[posColCartaUm][posLinCartaUm] == cartas[posColCartaDois][posLinCartaDois]){ //ACERTOU
            printf("\n\033[1;32m\tACERTOU\033[0m\n");
            cartas[posColCartaUm][posLinCartaUm] = '9';
            cartas[posColCartaDois][posLinCartaDois] = '9';

            pthread_mutex_lock(&mutex);
            if(playerUm + playerDois > 31) verificaPonto = 0;
            pthread_mutex_unlock(&mutex);

        } else if(veFinal) printf("\n\033[1;31m\tERROU\033[0m\n");

        if(veFinal) {
            char dummy_buffer[2];
            fgets(dummy_buffer, sizeof(dummy_buffer), stdin); // Ler e descartar a linha
            printf("\nPressione Enter para continuar...");
            fgets(dummy_buffer, sizeof(dummy_buffer), stdin); // Esperar pela próxima linha

                flag = 1;
                enter = '\0';
                for(i = 0; i < 8; i++){
                    for(j = 0; j < 8; j++){
                        transfereBuff[k] = cartas[i][j];
                        k++;
                    }
                }
                if(!verificaPonto) transfereBuff[64] = '0';
                k = 0;
        }
        strcpy(buff, transfereBuff);
        write(writefd, buff, 65);
    }
} // Fim da Funcao Server
