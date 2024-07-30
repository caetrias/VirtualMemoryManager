#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PageTable
{
    int pageNumber;
    int age;
} PageTable;

typedef struct TLB
{
    int pageNumber;
    int frame;
} TLB;


int getPageNumber(int logicalAddress);
int getOffset(int logicalAddress);
int isInPT(PageTable pageTable[], int pageNumber, int*frame);
int isInTLB(TLB tlb[], PageTable pageTable[], int pageNumber, int *frame, int *indexTLB, int *tlbHit);
void insertFifoPT(PageTable pageTable[], TLB tlb[], int pageNumber, int *posPT, int *frame);
void insertTLB(TLB tlb[], int pageNumber, int frame, int *posTLB, int *indexTLB);
void insertLruPT(PageTable pageTable[], int pageNumber, int *frame);
int getValue(int address);

int main(int argc, char **argv)
{

    PageTable pageTable[128];
    TLB tlb[16];

    // Inicializa PageTable com valores inválidos
    for (int i = 0; i < 128; i++) {
        pageTable[i].pageNumber = -1;
        pageTable[i].age = -1;
    }

    // Inicializa TLB com valores inválidos
    for (int i = 0; i < 16; i++) {
        tlb[i].pageNumber = -1;
        tlb[i].frame = -1;
    }

    int virtualAddress;

    //contadores para print
    int pageFault = 0; 
    int translatedAddresses = 0;
    int tlbHit = 0;

    // variaveis para guiar na insercao do fifo
    int posPT = 0;
    int posTLB = 0; 


    FILE *file = fopen(argv[1], "r");
    FILE *outputFile = fopen("correct.txt", "w");

    if(file == NULL || outputFile == NULL){
        return 1;
    }
    
    //FIFO
    if(strcmp("fifo", argv[2]) == 0){

        while (fscanf(file, "%d", &virtualAddress) != EOF)
        {     
            translatedAddresses++;

            //variavel para print da pagina
            int indexTLB = -1;

            //frame do VA atual, inicializa com valor invalido
            int frame = -1; 


            int pageNumber = getPageNumber(virtualAddress);
            int offset = getOffset(virtualAddress);

            //CHECA SE ESTA NA TLB
            int checkTLB = isInTLB(tlb, pageTable, pageNumber, &frame, &indexTLB, &tlbHit); //se achar, atualiza variavel frame
            if(checkTLB == 0){ //nao esta na TLB

                //CHECA SE ESTA NA PAGE TABLE
                int checkPT = isInPT(pageTable, pageNumber, &frame); //se achar, atualiza variavel frame

                //SE NAO ESTIVER NA PAGE TABLE
                if(checkPT == 0){

                    pageFault++;
                    insertFifoPT(pageTable, tlb, pageNumber, &posPT, &frame); //insere + atualiza variavel frame

                }

                insertTLB(tlb, pageNumber, frame, &posTLB, &indexTLB);

            }



            int physicalAddress = (frame * 256) + offset; 

            //int searchValue = virtualAddress*256;
            int value = getValue(virtualAddress);
            fprintf(outputFile, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", virtualAddress, indexTLB, physicalAddress, value);
        }

    }//FIM DO FIFO

    //LRU
    else if(strcmp("lru", argv[2]) == 0){

        while (fscanf(file, "%d", &virtualAddress) != EOF)
        {     
            translatedAddresses++;

            //percorre PT -> aumentando idade das pages
            for(int i=0; i<128 ; i++){
                pageTable[i].age++;
            }

            int indexTLB = -1;

            //frame do VA atual, inicializa com valor invalido
            int frame = -1; 

            int pageNumber = getPageNumber(virtualAddress);
            int offset = getOffset(virtualAddress);

            //CHECA SE ESTA NA TLB
            int checkTLB = isInTLB(tlb, pageTable, pageNumber, &frame, &indexTLB, &tlbHit); //se achar, atualiza variavel frame
            if(checkTLB == 0){ //nao esta na TLB

                //CHECA SE ESTA NA PAGE TABLE
                int checkPT = isInPT(pageTable, pageNumber, &frame); //se achar, atualiza variavel frame

                //SE NAO ESTIVER NA PAGE TABLE
                if(checkPT == 0){

                    pageFault++;
                    insertLruPT(pageTable, pageNumber, &frame); //insere + atualiza variavel frame

                }

                insertTLB(tlb, pageNumber, frame, &posTLB, &indexTLB);

            }



            int physicalAddress = (frame * 256) + offset; 

            //int searchValue = virtualAddress*256;
            int value = getValue(virtualAddress);
            fprintf(outputFile, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", virtualAddress, indexTLB, physicalAddress, value);
        }

    }

    float pageFaultRate = (float)pageFault/translatedAddresses;
    float tlbHitRate = (float)tlbHit/translatedAddresses;
    fprintf(outputFile, "Number of Translated Addresses = %d\nPage Faults = %d\nPage Fault Rate = %.3f\nTLB Hits = %d\nTLB Hit Rate = %.3f\n",translatedAddresses, pageFault, pageFaultRate, tlbHit, tlbHitRate);

    fclose(outputFile);
    fclose(file);
    return 0;
}

int getPageNumber(int virtualAddress) {
    return (virtualAddress >> 8) & 0xFF;
}

int getOffset(int virtualAddress) {
    return virtualAddress & 0xFF; 
} 

int isInTLB(TLB tlb[], PageTable pageTable[], int pageNumber, int *frame, int *indexTLB, int *tlbHit){

    for(int i=0; i<16; i++){

        if(pageNumber == tlb[i].pageNumber){
            (*tlbHit)++;

            *frame = tlb[i].frame;
            *indexTLB = i;
            pageTable[*frame].age = 0; //atualiza age na PT
            return 1; //achou e atualiza a variavel do frame da Pagina
        }

    }

    return 0;
}

//percorre PageTable e verifica se PageNumber esta ou nao dentro da mesma
int isInPT(PageTable pageTable[], int pageNumber, int *frame){

    for(int i=0;i<128;i++){

        if(pageNumber == pageTable[i].pageNumber){
            *frame = i;
            pageTable[i].age = 0;
            return 1; //achou e atualiza a variavel do frame da Pagina
        }
    }

    return 0; //nao achou
}

void insertTLB(TLB tlb[], int pageNumber, int frame, int *posTLB, int *indexTLB)
{

    tlb[*posTLB].pageNumber = pageNumber;
    tlb[*posTLB].frame = frame;
    *indexTLB = *posTLB;
    (*posTLB)++;

    if (*posTLB == 16) {
        *posTLB = 0; // Resetar quando atingir o tamanho máximo
    }
} 

void insertFifoPT(PageTable pageTable[], TLB tlb[], int pageNumber, int *posPT, int *frame)
{ // a cada insercao, posPT++

    //CHECA SE JA TEM PAGINA
    if(pageTable[*posPT].pageNumber != -1){

        //CHECA SE PAGINA ESTA NA TLB
        for(int i=0;i<16;i++){
            //REMOVE PAGINA DA TLB 
            if(pageTable[*posPT].pageNumber == tlb[i].pageNumber){
                //(PRECISA SER IMPLEMENTADO????)faz a reorganizacao da TLB, pois esse pagenumber sera removido
            }
        }

    }

    pageTable[*posPT].pageNumber = pageNumber;
    *frame = *posPT;
    (*posPT)++;

    if (*posPT == 128) {
        *posPT = 0; // Resetar quando atingir o tamanho máximo
    }
} 


void insertLruPT(PageTable pageTable[], int pageNumber, int *frame)
{
    int maiorAge = -1;
    int indexMaiorAge = -1;

    // percorre array até achar número com maior Age
    for(int i = 0; i < 128; i++){
        if(pageTable[i].pageNumber == -1){ // Encontra o primeiro slot vazio
            indexMaiorAge = i;
            break;
        }
        if(pageTable[i].age > maiorAge){
            maiorAge = pageTable[i].age;
            indexMaiorAge = i;
        }
    }

    // substitui página com maior age ou o primeiro slot vazio pela nova página (atualizando age da nova pagina)
    pageTable[indexMaiorAge].pageNumber = pageNumber;
    pageTable[indexMaiorAge].age = 0;
    *frame = indexMaiorAge;

}


int getValue(int address){
    FILE *file = fopen("BACKING_STORE.bin", "rb");
    if (file == NULL) {
        return -1;
    }

    if (fseek(file, address, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }
    signed char value;
    if (fread(&value, sizeof(signed char), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return (int)value;
}


