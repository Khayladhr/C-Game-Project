// file main.c
#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include "snake.c"

#define min(a,b) ((a) < (b) ? (a) : (b))  // mengembalikan nilai terkecil dari a dan b

// struct player
typedef struct 
{
    char username[25] ;
    int bestScore ;
} player ;

int MinHeight = 35, MinWidth = 80 ;
int heightT, widthT ;
int startX, startY = 0 ;
int iPlayer = -1 ;
bool lanjut ;
bool login = false ;
player playerNow ;

// inisialisasi warna untuk dipakai nanti
void initColor() {
    start_color() ;
    use_default_colors() ;
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
}

// inisialisasi ncurses (memulai program ncurses)
void initNcurses() {
    setlocale(LC_ALL, "");
    initscr() ;
    cbreak() ;
    noecho() ;
    curs_set(0) ;
}

// menggabungkan bagian-bagian yg sudah dibagi oleh mergeSort
void merge(player arr[], int l, int m, int r) {
    int n1 = m - l + 1 ;
    int n2 = r - m ;

    player L[n1], R[n2] ;

    for(int i=0; i<n1; i++) L[i] = arr[l + i] ;
    for(int i=0; i<n2; i++) R[i] = arr[m + i + 1] ;
    
    int i = 0, j = 0, k = l;

    while(i < n1 && j < n2) {
        if(L[i].bestScore >= R[j].bestScore) {
            arr[k] = L[i] ;
            i++ ;
        }else {
            arr[k] = R[j] ;
            j++ ;
        }
        k++ ;
    }

    while(i < n1) {
        arr[k] = L[i] ;
        i++ ;
        k++ ;
    }

    while(j < n2){
        arr[k] = R[j] ;
        j++ ;
        k++ ;
    }
}

// sorting merge untuk mengurutkan player dari score terbesar
void mergeSort(player arr[], int l, int r) {
    if(l < r) {
        int m = l + (r - l) /2 ;

        mergeSort(arr, l, m) ; 
        mergeSort(arr, m+1, r) ;

        merge(arr, l, m, r) ;
    }
}

// mencari username yang dimasukkan pengguna apakah ada di dataPlayer atau tidak (iPlayer = -1 jika tidak ada)
void cariUSN (char *targetName) {
    FILE *file = fopen("dataPlayer.dat", "rb+");
    if (!file) return;

    player temp;
    int index = 0 ;
    // Loop setiap data
    while (fread(&temp, sizeof(player), 1, file) == 1) {

        if (strcmp(temp.username, targetName) == 0) {
            iPlayer = index ; 
            playerNow.bestScore = temp.bestScore ;
            break;
        }
        index++ ;
    }

    fclose(file);
}

// menyimpan data player baru (player baru )
void saveNPlayer() {
    FILE *file = fopen("dataPlayer.dat", "ab");  // append binary (menambahkan data baru setelah data terahir)
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    fwrite(&playerNow, sizeof(player), 1, file);
    fclose(file);
}

// memperbarui score player di file dataPlayer
void saveNScore() {
    FILE *file = fopen("dataPlayer.dat", "rb+");  // read + write
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    // Pindah ke posisi index
    fseek(file, iPlayer * sizeof(player), SEEK_SET);

    // Tulis data baru
    fwrite(&playerNow, sizeof(player), 1, file);

    fclose(file);
}

// pesan bahwa untuk memulai game harus login
void loginMessage() {
    int mid = (widthT - 36 - 1 ) / 2  ;
    WINDOW * logM = newwin(3, 36, 14, mid) ;
    printWcolor(logM, 1, 1, "ANDA HARUS LOGIN TERLEBIH DAHULU!!", 3) ;
    refresh() ;
    wrefresh(logM) ;

    for(int i=0; i<5; i++) {
        colorBox(logM, 4) ;
        wrefresh(logM) ;
        napms(600) ;

        colorBox(logM, 3) ;
        wrefresh(logM) ;
        napms(400) ;
    }
    delWin(logM) ;  
}

// UI ketika user mengetikkan username
void inputUSN(char *usn, WINDOW * logP) {
    int index = 0 ;
    
    while (1) {
        int ch = wgetch(logP);
        
        // selesai ketika user menekan enter
        if (ch == '\n' || ch == KEY_ENTER) {
            break;
        }
        
        // backspace ditekan maka menghapus char di screen dan di array
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == 8) && index > 0) {
            index--;
            usn[index] = '\0';
            
            mvwprintw(logP, 2, 2 + index, " ");
            wmove(logP, 2, 2 + index);
            
            wrefresh(logP);
            continue;
        }
        
        // maksimal username 25
        if (index >= 25) continue;
        
        // hanya izinkan huruf/angka/spasi
        if (isprint(ch)) {
            usn[index] = ch;
            index++ ;
            mvwprintw(logP, 2, 2 + index - 1, "%c", ch);
            wrefresh(logP);
        }
    }

    usn[index] = '\0' ;
}

// tampilan untuk meminta konfirmasi membuat player baru (username baru)
bool konfirmNP() {
    int mid = (widthT - 50 - 1 ) / 2 ;
    WINDOW * newP = newwin(4, 50, 15, mid) ;
    keypad(newP, true) ;
    colorBox(newP, 3) ;
    printWcolor(newP, 0, 2, " USN anda tidak ada, ingin membuat USN baru ? ", 3) ;
    refresh() ;

    char *yn[] = {
        "YA",
        "TIDAK"
    };

    int highlight = 0 ;
    bool enter = false;
    
    while(!enter) {
        int x = 13 ;
        for(int i=0; i<2; i++) {
            if(i==highlight) {
                printWcolor(newP, 2, x, yn[i], 4) ;
            }else{
                printWcolor(newP, 2, x, yn[i], 3) ;
            }
            x = 30 ;
        }
        wrefresh(newP) ;
    
        int p = wgetch(newP) ;
        switch (p)
        {
        case KEY_LEFT:
            if(highlight==0) continue ;
            highlight-- ;
            break;
        
        case KEY_RIGHT:
        if(highlight == 1) continue ;
            highlight++ ;
            break;
        
        case '\n':
            enter = true ;
            break;
        
        default:
            break;
        }
    }
    
    delWin(newP) ;
    if(highlight == 0) return true ;
    else return false ;
}

// proses login
void loginProses() {
    int mid = (widthT - 30 - 1 ) / 2  ;
    WINDOW * logP = newwin(5, 30, 15, mid) ;
    colorBox(logP, 3) ;
    printWcolor(logP, 0, 2, " Masukkan USN (MAX 25) ", 3) ;
    charWcolor(logP, 2, 1, '>', 3) ;
    refresh() ;
    wrefresh(logP) ;
    
    char usn[30] ;
    memset(usn, 0, sizeof(usn)) ;
    inputUSN(usn, logP) ; // user memasukkan usn
    cariUSN(usn) ;  // mencari apakah username ada di dataPlayer (iPlayer = -1 jika tidak ada)

    if(iPlayer == -1) {
        if(!konfirmNP()){ // user tidak ingin membuat player baru
            delWin(logP) ;
            return ;   
        }else{  // user ingin membuat player baru
            playerNow.bestScore = 0 ;
            saveNPlayer() ;
            cariUSN(playerNow.username) ;  // memberikan nilai ke iPlayer (untuk save score terbaru)
        }
    }
    
    login = true ;
    strcpy(playerNow.username, usn); 
    napms(700) ;
    delWin(logP) ;
}

// memperlihatkan leaderboard 10 player dengan score terbesar
void leaderboard(int WidthM) {
    FILE *file = fopen("dataPlayer.dat", "rb");  // read
    if (!file) return;

    player allPlayer [100] ;
    player tmp ;
    int index = 0 ;

    // membaca dataPlayer dan menyimpan di array allPlayer
    while (fread(&tmp, sizeof(player), 1, file) == 1) {
        allPlayer[index] = tmp ;
        index++ ;
    }

    fclose(file) ;

    if(index == 0)return ;

    mergeSort(allPlayer, 0, index-1) ; // mengurutkan player 
    
    char *leaderText[] = {
        "█░░ █▀▀ █▀▀█ █▀▀▄ █▀▀ █▀▀█ █▀▀▄ █▀▀█ █▀▀█ █▀▀█ █▀▀▄",
        "█░░ █▀▀ █▄▄█ █░░█ █▀▀ █▄▄▀ █▀▀▄ █░░█ █▄▄█ █▄▄▀ █░░█",
        "▀▀▀ ▀▀▀ ▀░░▀ ▀▀▀░ ▀▀▀ ▀░▀▀ ▀▀▀░ ▀▀▀▀ ▀░░▀ ▀░▀▀ ▀▀▀░"
    };

    int m = (WidthM-2-53)/2 + startX ;
    WINDOW * leaderTitle = newwin(5, 53, 4, m) ;
    for(int i=0; i<3; i++){
        printWcolor(leaderTitle, i+1, 1, leaderText[i], 3) ;
    }
    
    refresh() ;
    wrefresh(leaderTitle) ;

    m = (WidthM-2-35)/2 + startX ;
    WINDOW * leaderP = newwin(21, 35, 11, m) ;
    colorBox(leaderP, 3) ;
    mvwprintw(leaderP, 20, 2, " Press any key to return " );
    refresh() ;
    
    // mencetak player 1 per 1
    for(int i=0; i<min(index, 10); i++) {
        mvwprintw(leaderP, i+1+i, 1, "%s", allPlayer[i].username) ;

        int j ;
        for(j=strlen(allPlayer[i].username)+1; j<=30; j++) {
            mvwprintw(leaderP, i+1+i, j, " ") ;
        }
        mvwprintw(leaderP, i+1+i, j, "%d", allPlayer[i].bestScore) ;
    }
    wrefresh(leaderP) ;

    wgetch(leaderP) ;
    delWin(leaderTitle) ;
    delWin(leaderP) ;
}

// mencetak ular di menu utama
void titleUI(int WidthM, WINDOW * titleWin) {

    // buat title
    char *title[] = {
        "▄▄▀█▄   ▄       ▄",
        "▀▀▀██──███ ─── ███",
        "░▄██▀░█████░░░█████░░",
        "███▀▄███░███░███░███░▄",
        "▀█████▀░░░▀███▀░░░▀██▀"
    };
    
    for(int i=0; i<5; i++) {
        printWcolor(titleWin, i+1, 1, title[i], 3) ;
    }
    wrefresh(titleWin) ;
}

// menampilkan tampilan game over ketika ular mati
void gameOver() {
    // deklarasi string ASCII yang akan dicetak
    
    char *text_gameOver[] = {
        "░██████╗░░█████╗░███╗░░░███╗███████╗  ░█████╗░██╗░░░██╗███████╗██████╗░",
        "██╔════╝░██╔══██╗████╗░████║██╔════╝  ██╔══██╗██║░░░██║██╔════╝██╔══██╗",
        "██║░░██╗░███████║██╔████╔██║█████╗░░  ██║░░██║╚██╗░██╔╝█████╗░░██████╔╝",
        "██║░░╚██╗██╔══██║██║╚██╔╝██║██╔══╝░░  ██║░░██║░╚████╔╝░██╔══╝░░██╔══██╗",
        "╚██████╔╝██║░░██║██║░╚═╝░██║███████╗  ╚█████╔╝░░╚██╔╝░░███████╗██║░░██║",
        "░╚═════╝░╚═╝░░╚═╝╚═╝░░░░░╚═╝╚══════╝  ░╚════╝░░░░╚═╝░░░╚══════╝╚═╝░░╚═╝"
    };
    
    WINDOW * temp ;
    
    temp = newwin(10, 73, startY+8, startX+3) ;
    
    for(int i=5; i>0; i--) {  // for loop hitung mundur
        colorBox(temp, 3) ;
        mvwprintw(temp, 9, (72-48)/2, " Anda akan kembali ke menu utama dalam %d detik ", i) ;
        
        for (int j=0; j<6; j++) {   // for loop cetak UI game over
            printWcolor(temp, j+2, 1, text_gameOver[j], 3) ;
        }
        refresh() ;
        wrefresh(temp) ;
        
        // flickering
        napms(750) ;
        for (int j=1; j<=8; j++) {
            for(int k=1; k<=71; k++) {
                mvwaddch(temp, j, k, ' ') ;
            }
        }
        wrefresh(temp) ;
        napms(250) ;
    }
    delWin(temp) ;
}

// menampilkan pilihan di menu utama dan menerima input (keypad)
int pilihanMenu(int WidthM, WINDOW * mainMenu) {
    // pilihan
    char *choices [] = {
        "New Game",
        "Leaderboard",
        "Quit",
        "Login"
    };
    
    if(login) choices[3] = "Logout" ;

    int highlight = 0 ;
    int enter = false ;
    int m = (WidthM-2-17)/2 + startX ;
    WINDOW * pilihan = newwin(11, 17, 15, m) ;
    keypad(pilihan, true) ;
    refresh() ;
    
    // jika sudah login tampilkan username di kiri atas 
    if(login) mvwprintw(mainMenu, 0, 3, " %s ", playerNow.username) ;
    wrefresh(mainMenu) ;
    
    while(!enter) {
        for(int i=0; i<4; i++) {
            if(i==highlight) {
                printWcolor(pilihan, i+1+i, (15-strlen(choices[i])-1) / 2 + 1, choices[i], 4) ;
            } else {
                printWcolor(pilihan, i+1+i, (15-strlen(choices[i])-1) / 2 + 1, choices[i], 3) ;
            }
        }

        wrefresh(pilihan) ;
        
        int g = wgetch(pilihan) ;
        
        switch (g)
        {
            case KEY_UP:
            highlight-- ;
            break;
            case KEY_DOWN:
            highlight++ ;
            break;
            case '\n':
            enter = true ;
            break ;
            default:
            break;
        }
        highlight%=4 ;
        if(highlight<0)highlight = 3 ;
    }
    
    napms(1000) ;
    delWin(pilihan) ;
    return highlight ;
}

// menampilkan menu utama, titleUI dan pilihanMenu digunakan disini
void menuUtama() {
startGame1:
    // mendapatkan ukuran terminal user
    getmaxyx(stdscr, heightT, widthT) ;
    
    int HeightM = 35, WidthM = 80 ;

    // jika terminal user kurang dari minimal maka program tidak lanjut dan memberikan pesan dibawah
    if(heightT < MinHeight || widthT < MinWidth){
        endGame() ;
        fprintf(stderr,
            "Terminal anda terlalu kecil: %d (Horizontal) x %d (Vertikal).\n"
            "Perbesar terminal anda setidaknya %d (Horizontal) x %d (Vertikal) lalu coba lagi.\n",
            widthT, heightT, MinWidth, MinHeight);
            exit(0);
    }
        
    startX = (widthT - gameWidth - 1 ) / 2 ; 
    
    // buat window untuk main menu
    WINDOW * mainMenu = newwin(HeightM, WidthM, startY, startX) ;
    refresh() ;

startGame2:
    int m = (WidthM-2-24)/2 + startX ;
    WINDOW * titleWin = newwin(7, 24, 4, m) ;
    refresh() ;
    
startGame3:
    colorBox(mainMenu, 3) ;
    wrefresh(mainMenu) ;
    titleUI(WidthM, titleWin) ;
    switch (pilihanMenu(WidthM, mainMenu))
    {
        case 0:  // main ular 
        if(!login) {
            loginMessage() ;
            goto startGame3 ;
        } 
        delWin(mainMenu) ;
        delWin(titleWin) ;
        initSnake() ;

        while(lanjut) {
            napms(120) ;
            input() ;
            
            update() ;
            if(!lanjut) break ;
            draw() ;
            
            
            napms(50) ;
            input() ;
            update() ;
            draw() ;
        }

        gameOver() ;
        if(playerNow.bestScore < score) {
            playerNow.bestScore = score ; // memperbarui best score jika score lebih besar
            saveNScore() ; // menyimpan score baru
        }
        wclear(snakeWin) ;
        wrefresh(snakeWin) ;
        goto startGame1 ; // kembali ke menu utama
        break;
        
        case 1:  // menampilkan leaderboard
            delWin(titleWin) ;
            leaderboard(WidthM) ;
            goto startGame2 ;  // kembali ke menu utama
            break ;
        case 2:  //quit game
            delWin(mainMenu) ;
            delWin(titleWin) ;
            endGame() ;
            break ;
        case 3:  // login & logout
            if(login) {
                login = false ;
                iPlayer = -1 ;
            }else{
                loginProses() ;
            }
            goto startGame3 ;  // kembali ke menu utama
            break ;
        default:
        break;
    }
}












// ini bagian menu file menu.c

#include "menu.c"

int main() {
    initNcurses() ;
    initColor() ;
    menuUtama() ;
    return 0;
}














// ini untuk pergerakannya file snake.c

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

extern int startY, startX ;
extern bool lanjut ;

typedef struct {
    int x ;
    int y ;
} koordinat ;

int gameHeight = 25, gameWidth = 80 ;
int score ;
int pBadan ;
koordinat kepala, apel, gerak, badan[1000] ;
WINDOW * snakeWin ;

// mencetak (bisa semua tipe data) dengan warna
void printWcolor(WINDOW * curWin, int y, int x, char *s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwprintw(curWin, y, x, "%s", s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

// mencetak karakter berwarna
void charWcolor(WINDOW * curWin, int y, int x, char s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwaddch(curWin, y, x, s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

// membuat border berwarna
void colorBox(WINDOW * curWin, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    box(curWin, 0, 0) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

// menghapus WINDOW yang sudah tidak digunakan
void delWin(WINDOW * tmp) {
    wclear(tmp) ;
    wrefresh(tmp) ;
    delwin(tmp) ;
    refresh() ;
}

// cek apakah 2 karakter (ular dan apel) berada di posisi yang sama
bool cek1 (koordinat a, koordinat b) {
    if(a.x != b.x) return false ;
    if(a.y != b.y) return false ;
    
    return true ;
}

// memeriksa apakah posisi kepala ular saat ini mati (kena dinding dan badan ular)
bool cek_mati() {
    for(int i=0; i<pBadan; i++) {
        if(kepala.x == badan[i].x && kepala.y == badan[i].y) {
            lanjut = false ;
            return true ;
        }
    }

    if(kepala.x == 0 || kepala.x == gameWidth-1 || kepala.y == 0 || kepala.y == gameHeight-1){
        lanjut = false ;
        return true ;
    }
    return false ;
}

// spawn apel baru serta memastikan koordinat tidak sama dengan ular
void spawn_apel() {
    apel.x = 1 + rand() % (gameWidth - 2) ; 
    apel.y = 1 + rand() % (gameHeight - 2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (gameWidth - 2); 
        apel.y = 1 + rand() % (gameHeight - 2) ;
    }
}

// mempersiapkan game ular (WINDOW, variabel, dll)
void initSnake() {
    lanjut = true ;
    pBadan = 0 ;
    score = 0 ;
    snakeWin = newwin(gameHeight, gameWidth, startY, startX) ;
    box(snakeWin, 0, 0) ;
    refresh() ;
    wrefresh(snakeWin) ;
    keypad(snakeWin, true) ;
    nodelay(snakeWin, true) ;
    srand(time(NULL)) ;
    
    kepala.x = startX + 1, kepala.y = startY + 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;
    
    charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    wrefresh(snakeWin) ;
}

// game selesai, mengakhiri ncurses dan menampilkan kursor kembali
void endGame() {
    curs_set(1) ;
    endwin() ;
}



void input() {
    int x = wgetch(snakeWin) ;
    
    switch (x)
    {
        case KEY_UP:
        if(gerak.y == 1) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
        case KEY_DOWN:
        if(gerak.y == -1) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
        case KEY_RIGHT:
        if(gerak.x == -1) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
        case KEY_LEFT:
        if(gerak.x == 1) return ;
        gerak.x = -1 ;
        gerak.y = 0 ;
        break;
        
        case '\e':
        endGame() ;
        exit(0) ;
        break;
        
        default:
        break;
    }
}

void update() {
    if(cek1 (apel, kepala)) {
        spawn_apel() ;
        score++ ;
    }
    
    if(cek_mati()){
        return ;
    }
    
    mvwaddch(snakeWin, kepala.y, kepala.x, ' ') ;
    if(pBadan > 0) mvwaddch(snakeWin, badan[pBadan-1].y, badan[pBadan-1].x, ' ') ;
    wrefresh(snakeWin) ;
    
    if(pBadan > 0) {
        for (int i=pBadan-1; i>0; i--) {
            badan [i] = badan [i-1] ;
        }
        badan[0] = kepala ;
    }
    if(score/1 > pBadan) {
        if(pBadan == 0) {
            badan [pBadan].y = kepala.y - gerak.y ;
            badan [pBadan].x = kepala.x - gerak.x ;
        }else {
            badan [pBadan].y = badan[pBadan-1].y - gerak.y ;
            badan [pBadan].x = badan[pBadan-1].x - gerak.x ;
        }
        pBadan++ ;
    }
    kepala.x += gerak.x ;
    kepala.y += gerak.y ;
}


void draw() {
    if(gerak.x == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    }else if (gerak.x == -1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '<', 2) ;
    } else if (gerak.y == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, 'v', 2) ;
    } else {
        charWcolor(snakeWin, kepala.y, kepala.x, '^', 2) ;
    }
    
    for(int i=0; i<pBadan; i++) {
        printWcolor(snakeWin, badan[i].y, badan[i].x, "●", 2) ;
    }
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    colorBox(snakeWin, 3) ;
    mvwprintw(snakeWin, 0, (gameWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(snakeWin) ;
}










// Ini dataplayer nya file dataplayer.dat
Epan                                           nanda                       
   kayla                          deanda                         
