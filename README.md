##Virtual Memory Manager

<h3>Do que se trata o projeto?</h3>

Este projeto se trata de uma atividade da cadeira de Infraestrutura de Software e corresponde à implementação do "Designing a Virtual Memory Manager" apresentado no livro "Operating System Concepts, Silberschatz A. et al". Contudo, possuindo algumas modificações, como:

- A implementação deverá ser aquela em que a memória física tem apenas 128 frames.
  
- O programa deve ser implementado em C e ser executável em sistemas Linux, com a compilação feita por Makefile, através simplesmente do comando make via terminal, e retornar o arquivo com nome vm executável;

- Para validação da implementação, será utilizado o compilador gcc 13.2.0, com o arquivo de saída seguindo rigorosamente a formatação e texto apresentados.

- Os frames na memória física devem ser preenchido do 0 ao 127, e quando a memória estiver cheia, aplicasse o algoritmo de substituição a fim de identificar qual frame será atualizado;

- Deve-se implementar dois algoritmos de substituição de página, a saber fifo e lru, enquanto que na TLB será aplicado apenas o fifo;

- O primeiro argumento por linha de comando será um arquivo de endereços lógicos, o segundo argumento será o tipo de algoritmo a ser utilizado para substituição da página (fifo ou lru). Por exemplo, a chamada:

                "./vm address.txt lru"
  
indica que o algoritmo de substituição da página será o lru.
- O arquivo de saída será denominado como correct.txt, seguindo rigorosamente a mesma
formatação do que foi anexado na atividade do Classroom, que no caso utilizou o fifo; 


<h3>Como Utilizar?</h3>
1. Dentro diretório do projeto, abra um terminal linux
2. Digite "make" -> para compilar
3. "./vm address.txt fifo" ou "./vm address.txt lru" 
<i>Dentro de "test_cases.rar" é possível encontrar mais arquivos address.txt, e seus respectivos arquivos os quais representam a saída desejada</i>
