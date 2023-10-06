#include <stdio.h>
#include "lib_ppm.h"
#include <limits.h>
#include <string.h>


// função que retorna o byte 1 com seus dois ultimos bits menos representativos troccados pela dupla de bits escolhida do byte 2.
unsigned char trocarBits(unsigned char byte1, unsigned char byte2, int contador) {


    // Limpar os dois últimos bits de byte 1
    byte1 &= ~0b11;

    //"empurra" para a direita os bits. Quatidade definida pelo contador. bitsByte2 ficara com os dois bits selecionados armazenados.
    unsigned char bitsByte2 = (byte2 >> contador) & 0x03;

    // combina byte1 e bitsbyte2
    byte1 |= bitsByte2;


    return byte1;
}

// função que retorna os dois ultimso bits extraidos do byte enviado
unsigned char extraiUltimosDoisBits(unsigned char byte1) {
    // Extrair os dois últimos bits de 'byte1'
    return byte1 & 0b11;
}



int main(){ 

    //verifica qual operacao o usuario deseja fazer. 1 = codificador das imagens. 2 = decodificador de uma imagem
    printf("Qual operação deseja fazer: \n1-Códificar duas imagens \n2-extrair imagem de outra\n");
    int escolha =0;
    scanf("%d", &escolha);

    if(escolha == 1) {

    //le o nome do arquivo da imagem base
    char  imagemBase[20];
    printf("Nome do arquivo da imagem que sera usada para esconde a outra imagem: ");
    scanf("%s", imagemBase);

    // leitura do arquivo da imagem base
    Img data;
    Img *image = &data;
    int r = read_ppm(imagemBase, image);

    // le o nome do arquivo da imagem a ser escondida
    char imagemASerEscondida[20];
    printf("\nNome do arquivo da imagem que deseja esconder: ");
    scanf("%s", imagemASerEscondida);
    
    // leitura do arquivo da imagem a ser escondida
    Img dataSecond;
    Img *secondImage = &dataSecond;    
    int new = read_ppm(imagemASerEscondida, secondImage);

    // verifica se os arquivos foram encintrados
    if(r == -1 || new == -1) {
        printf("Arquivos de imagem não encontrados. Verifique a sua escrita e sua existencia na pasta do progrma");
        return 0;
    }

    // verifica se a imagem a ser escondida cabe na imagem base
    if(((image->height * image->width) / 4) < (secondImage->height * secondImage->width)) {
    printf("tamanhos de imagem incompativeis. A imagem base deve ser 4x vezes maior que a imagem a ser escondida");
    return 0;
    }

    // utilizado para selecionar a dupla de bits que sera mexida 
    int contador = 0;

    // inserção da informação de altura da imagem a ser escondia nos ultimos dois bits dos dois primeiros pixels da imagem base
    for (int i = 0; i < 2; i++) {
        // Extrair e armazenar dois bits de cada byte mais significativo no array de unsigned char
        image->pix[0 * image->width + i].r = (unsigned char)(secondImage->height >> ((5 - contador) * 2)) & 0b11; 
        contador++;
        image->pix[0 * image->width + i].g = (unsigned char)(secondImage->height >> ((5 - contador) * 2)) & 0b11; 
        contador++;
        image->pix[0 * image->width + i].b = (unsigned char)(secondImage->height >> ((5 - contador) * 2)) & 0b11; 
        contador++;
    }

     // utilizado para selecionar a dupla de bits que sera mexida 
    contador = 0;

    // inserção da informação da largura da imagem a ser escondia nos ultimos dois bits dos dois primeiros pixels da imagem base
    for (int i = 2; i < 4; i++) {
        image->pix[0 * image->width + i].r = (unsigned char)(secondImage->width >> ((5 - contador) * 2)) & 0b11; 
        contador++;
        image->pix[0 * image->width + i].g = (unsigned char)(secondImage->width >> ((5 - contador) * 2)) & 0b11; 
        contador++;
        image->pix[0 * image->width + i].b = (unsigned char)(secondImage->width >> ((5 - contador) * 2)) & 0b11; 
        contador++;
    }
   
   
    

    // seleção da dupla de bits a ser manipulada
    contador = 6;
    // infomacoes sobre qual pixel da imagem a ser escondida estamos amnipulando
    int largura = 0;
    int altura = 0;
    // utilizado apenas para pularamos os pixels que contem as informacoes de altura e largura da imagem que esta escondida 
    int v = 0;

    // loop para a altura da imagem base
    for(int t = 0; t < image->height ; t++) {
        // para pular os primeiros pixels da imagem que contem as informacoes de altura e largura da imagem
        if(t == 0) {
            v = 4;
        }
        else{
            v = 0;
        }

        // loop para a largura da imagem 
        for(int i = v; i < image->width; i++) {
        
        //armazena as informacoes de red da imagem a ser escondida nos dois bits menos representativos do pixel manipulado da inmagem base
        image->pix[t * image->width + i].r = trocarBits(image->pix[t * image->width + i].r, secondImage->pix[altura * secondImage->width + largura].r, contador );
 
        //armazena as informacoes de green da imagem a ser escondida nos dois bits menos representativos do pixel manipulado da inmagem base
        image->pix[t * image->width + i].g = trocarBits(image->pix[t * image->width + i].g, secondImage->pix[altura * secondImage->width + largura].g, contador );

        //armazena as informacoes de blue da imagem a ser escondida nos dois bits menos representativos do pixel manipulado da inmagem base
        image->pix[t * image->width + i].b = trocarBits(image->pix[t * image->width + i].b, secondImage->pix[altura * secondImage->width + largura].b, contador);

        // passa para a proxima dupla de bits do pixel sendo manipulado da imagem a ser escondida
        contador = contador - 2;

        //sistema que verifica se todos os bits do pixel da imagem secreta manipulado no momento ja foram armazenados, sendo assim passa para o proximo pixel.
        
        if(contador < 0) {
        // inicia de novo a varredura da dupla de bits
            contador= 6;
        // passa para o proximo pixel de largura
            largura++;
        } 
        // veirifca se ja "varremos" todos os pixels da imagem a ser escondida, sendo assim, aumentamos a altura e zeramos a largura para varrer a proima altura da imagem
        if(largura > secondImage->width) {
            altura++;
            largura = 0;
        }
       
       }
       

     
    }

    // grava em ppm a imagem base com a imnagem secreta inserida 
    write_ppm("imagemGerada.ppm", image);

    // caso o usuario deseje descodificar a imagem 
    } else if(escolha == 2) {

    // le qual arquivo que deseja ser descodificado
    char  imagemBase[20];
    printf("Imagem a ser descodificada: ");
    scanf("%s", imagemBase);

    // leitura do arquivo
    Img data;
    Img *image = &data;
    int r = read_ppm(imagemBase, image);

    // extração das informações de altura e largura da immagem escondida
    unsigned int alturaDaImagemEscondida = 0;
    unsigned int larguraDaImagemEscondida = 0;
   
   // extrai os dois ultimos bits dos dois primeiros pxiels da imagem que esta sendo desdoficada. Lembrando que esses dois primiros pixel contem a informacao de altura da imagems secreta
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 0].r << ((5 - 0) * 2)); // Inserir 2 bits no número novo
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 0].g << ((5 - 1) * 2)); // Inserir 2 bits no número novo
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 0].b << ((5 - 2) * 2)); // Inserir 2 bits no número novo
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 1].r << ((5 - 3) * 2)); // Inserir 2 bits no número novo
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 1].g << ((5 - 4) * 2)); // Inserir 2 bits no número novo
    alturaDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 1].b << ((5 - 5) * 2)); // Inserir 2 bits no número novo

    // extrai os dois ultimos bits do terceiro e quarto pixels da imagem que esta sendo descodificada. Lembrando que esses pixels possuem a informacao de largura da imegm secreta
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 2].r << ((5 - 0) * 2)); // Inserir 2 bits no número novo
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 2].g << ((5 - 1) * 2)); // Inserir 2 bits no número novo
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 2].b << ((5 - 2) * 2)); // Inserir 2 bits no número novo
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 3].r << ((5 - 3) * 2)); // Inserir 2 bits no número novo
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 3].g << ((5 - 4) * 2)); // Inserir 2 bits no número novo
    larguraDaImagemEscondida |= ((unsigned int)image->pix[0 * image->width + 3].b << ((5 - 5) * 2)); // Inserir 2 bits no número novo

    
    // cria um novo arquivo ppm. Nele sera remontada a imagem secreta
    Img novaImagemdata;
    Img *novaImagem= &novaImagemdata;
    r = new_ppm(novaImagem, larguraDaImagemEscondida, alturaDaImagemEscondida);

    // variaveis auxiliares para remontar a imagem
    unsigned char red = 0;
    unsigned char green =0;
    unsigned char blue =0;

    // variaveln utilizada para conbtar quantos pixels da imagem sendo descodificada foram percorridos
    int contador = 1;
    // variaveis que contam em que pixel esta a imagem sendo recriada
    int largura = 0;
    int altura = 0;

    int v = 0;

    // loop para altura da imagem sendos descodificada
    for(int t = 0; t < image->height; t++) {
        // apenas para pular os primeiros pixels da imagem sendo descodificada ja que possuem informacoes de altura e largura da imagem secreta
        if(t == 0) {
            v = 4;
        }
        else{
        v = 0;
        }
      
        // loop para a largura da imagem sendo descodificada
        for(int i = v; i < image->width ; i++) {
            
            //extrai os ultimos dois bits do pixel da imagem 
            red = (red << 2) | extraiUltimosDoisBits(image->pix[t * image->width + i].r);
            green = (green << 2) | extraiUltimosDoisBits(image->pix[t * image->width + i].g);
            blue = (blue << 2) | extraiUltimosDoisBits(image->pix[t * image->width + i].b);

            // verifica se ja varremos todos bits do pixel da imgame secreta    
            if(contador == 4) {
                //armazena na niva imagem os bits do pixel da imagem remontada
                novaImagem->pix[altura * novaImagem->width + largura].r = red;
                novaImagem->pix[altura * novaImagem->width + largura].g = green;
                novaImagem->pix[altura * novaImagem->width + largura].b = blue;
                //passa opara proximo pixel de largura
                largura++;
                contador = 1;

            //caso ainda falte bits pra varrer
            } else {
                contador++;
            }
            // caso chegamos no tamanho da largura da imagem escondida
            if(largura > larguraDaImagemEscondida) {
                altura++;
                largura = 0;
            }    

        }
   
}
        // escreve a imagem remontada
        write_ppm("imagemEscondida.ppm", novaImagem);

    }
    
    return 0;
}

