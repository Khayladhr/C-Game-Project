# C-Game-Project
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// akan digunakan untuk memberikan warna ke suatu teks
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

int n;                   // variabel untuk baris, kolom, dan jumlah bom
int jumlah_kotak_menang; // vaariabel untuk menyimpan jumlah kotak yang harus dibuka untuk menang
bool grid1[15][15];      // matrix untuk menentukan sebuah kotak sudah terbuka atau belum
int grid2[15][15];       // matrix untuk menyimpan jumlah bom disekitar di setiap kotak

char border[] = "=====================================";
int nyawa;
int hint;

// fungsi untuk membersihkan terminal
void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// fungsi awal untuk menentukan tingkat kesulitan
void intro()
{
    int input_level;
    clear_screen();

    printf("%s\n", border);
    printf("==========TINGKAT KESULITAN==========\n");
    printf("%s\n", border);
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");

    printf("Pilih Tingkat Kesulitan Yang Anda Inginkan: ");
    scanf("%d", &input_level); // meminta input tingkat kesulitan yang diinginkan

    // menentukan tingkat kesulitan dari input yang diberikan menggunakan switch
    switch (input_level)
    {
    case 1:
        n = 5;
        nyawa = 2;
        hint = 2;
        break;
    case 2:
        n = 7;
        nyawa = 5;
        hint = 5;
        break;
    case 3:
        n = 9;
        nyawa = 7;
        hint = 7;
        break;
    }

    // setelah baris, kolom, dan jumlah bom sudah ditentukan
    // harus melakukan perhitungan jumlah kotak (bukan berisi bom) yang harus dibuka untuk menang
    jumlah_kotak_menang = (n * n) - n;
}

// fungsi yang bertujuan untuk menjelaskan cara bermain
void rules()
{
    clear_screen();
    printf("%s\n", border);
    printf("================RULES================\n");
    printf("%s\n", border);
    printf("1. Buka semua kotak tanpa menyentuh bom.\n");
    printf("2. Kotak hijau = belum dibuka.\n");
    printf("3. Kotak angka = jumlah bom di sekitar.\n");
    printf("4. Ketik nomor kotak untuk membuka.\n");
    printf("5. Kamu punya sejumlah nyawa tergantung level.\n");
    printf("6. Kamu punya sejumlah hint tergantung level.\n");
    printf("7. Ketik 0 untuk memakai hint.\n");
    printf("%s\n", border);

    // memastikan pemain ingin melanjutkan permainan setelah membaca rules
    char lanjut;
    printf("Lanjut ke permainan? [y/n] : ");
    scanf(" %c", &lanjut);

    // jika input pemain adalah n maka program berahir
    if (lanjut == 'n')
    {
        clear_screen();
        exit(0); // program berahir, seperti return(0)
    }
}

// fungsi untuk menghitung jumlah bom di sekitar kotak baris ke i kolom ke j
int cek_sekitar(int i, int j)
{
    // memeriksa apakah kotak berisi bom
    // jika iya maka tidak perlu menghitung jumlah bom di sekitar
    if (grid2[i][j] == -1)
        return -1;
    int ans = 0; // variabel untuk menyimpan jumlah bom yang mula mula jml bom = 0

    // memeriksa kotak-kotak di sekitarnya, jika ada bom maka jumlah bertambah
    if (grid2[i][j + 1] == -1)
        ans++; // memeriksa bagian samping kanan
    if (grid2[i + 1][j + 1] == -1)
        ans++; // memeriksa bagian kanan bawah
    if (grid2[i + 1][j] == -1)
        ans++; // memeriksa bagian bawah
    if (grid2[i + 1][j - 1] == -1)
        ans++; // memeriksa bagian kiri bawah
    if (grid2[i][j - 1] == -1)
        ans++; // memeriksa bagian samping kiri
    if (grid2[i - 1][j - 1] == -1)
        ans++; // memeriksa bagian kiri atas
    if (grid2[i - 1][j] == -1)
        ans++; // memeriksa bagian atas
    if (grid2[i - 1][j + 1] == -1)
        ans++; // memeriksa bagian kanan atas

    // mengembalikan jumlah bom disekitar kotak
    return ans;
}

// fungsi untuk menyimpan jumlah bom di sekitar setiap kotak
void inisialisasi_grid2()
{
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            grid2[i][j] = cek_sekitar(i, j);
        }
    }
}

// mencetak pengumuman bahwa pemain kalah
void game_over()
{
    printf("\n%s\n", border);
    printf("==============GAME OVER==============\n");
    printf("%s\n", border);
}

// mencetak pengumuman bahwa pemain menang
void menang()
{
    printf("\n%s\n", border);
    printf("=========SELAMAT ANDA MENANG=========\n");
    printf("%s\n", border);
}

// fungsi untuk mencetak seluruh kotak, sisa hint, dan sisa nyawa selama permainan berlangsung
void cetak()
{
    int no_kotak = 1; // variabel untuk memberi nomor kotak dari paling kiri atas ke paling kanan bawah

    // cetak kotak satu per satu
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {

            // memeriksa apakah kotak sudah dibuka
            if (!grid1[i][j])                         // belum dibuka
                printf(GREEN "%3d " RESET, no_kotak); // akan menghasilkan no kotak berwarna hijau (belum dibuka)
            else
            {
                if (grid2[i][j] == -1)
                    printf(RED "  X " RESET); // akan menghasilkan X yang berarti bom
                else
                    printf("%3d ", grid2[i][j]); // akan menghasilkan jumlah bom yang ada di sekitarnya
            }
            no_kotak++;
        }
        printf("\n");
    }
    printf("%s\n", border);
    printf("Nyawa: %d | Hint tersisa: %d\n", nyawa, hint);
    printf("%s\n", border);
}

// fungsi untuk menyimpan nilai kotak yang akan dibuka
// dan memeriksa apakah kotak yang dibuka berisi bom
void buka(int x)
{
    int no_kotak = 1;
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {

            // memeriksa apakah no kotak sekarang sudah sesuai dengan no kotak yang ingin dibuka dan kondisi kotak tidak boleh terbuka
            if (no_kotak == x && grid1[i][j] == false)
            {
                grid1[i][j] = true; // menandakan kotak sudah dibuka

                // jika kotak yang dibuka berisi bom maka nyawa akan berkurang dan akan game over jika nyawa sudah habis
                // jika tidak jumlah kotak yang harus dibuka agar menang akan berkurang
                if (grid2[i][j] == -1)
                {
                    nyawa--;
                    printf(RED "Kamu menyentuh bom! Nyawa berkurang jadi %d.\n" RESET, nyawa);

                    // jika nyawa habis maka game over dan program berhenti
                    if (nyawa <= 0)
                    {
                        game_over();
                        for (int i = 1; i <= 8e8; i++)
                            ;
                        clear_screen();
                        exit(0);
                    }
                }
                else
                {
                    jumlah_kotak_menang--;
                }
            }
            no_kotak++;
        }
    }
}

// fungsi untuk memberi pemain kotak yang bukan berisi bom
// caranya dengan memeriksa kotak yg belum dibuka sesuai urutan nomor
// dan nomor kotak pertama yang ditemukan aman akan dicetak
void pakai_hint()
{
    // periksa apakah masih ada kesempatan hint
    if (hint <= 0)
    {
        printf(RED "Hint sudah habis!\n" RESET);
        return; // mengahiri fungsi dan kembali ke fungsi pemanggil
    }

    int aman; // variabel untuk menyimpan nomor kotak yang aman
    int no_kotak = 1;

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            // memeriksa apakah kotak belum dibuka dan apakah kotak bukan berisi bom
            if (!grid1[i][j] && grid2[i][j] != -1)
            {
                aman = no_kotak;
                break;
            }
            no_kotak++;
        }
        if (aman != -1)
            break;
    }

    // cetak no kotak aman yang ditemukan
    printf(GREEN "Hint: Kotak nomor %d tidak ada bom!\n" RESET, aman);
    hint--;
}

// fungsi untuk membaca dan mengatur apa yang akan dilakukan
// dari input yang diberikan pemain ketika game sudah dimulai
void mainn()
{
    int kotak_berapa;
    printf("Masukkan Kotak yang dipilih (0 = hint): ");
    scanf("%d", &kotak_berapa); // untuk membaca kotak berapa yang ingin dibuka atau ingin menggunakan hint

    // memeriksa jika pemain meminta hint
    if (kotak_berapa == 0)
    {
        pakai_hint();
        return; // mengahiri fungsi dan kembali ke fungsi pemanggil
    }

    // buka kotak yang dipilih
    buka(kotak_berapa);

    // menampilkan seluruh kotak setelah kotak yang dipilih terbuka
    clear_screen();
    cetak();
}

// fungsi untuk mengatur alur game dari awal hingga akhir
// kenapa tidak di int main () ?
// jika pemain ingin bermain lagi setelah sebuah game selesai maka tinggal memanggil fungsi ini saja
void main1()
{
    intro();
    rules();
    clear_screen();

    memset(grid1, 0, sizeof(grid1)); // memberi nilai 0 atau false yang artinya tertutup
    memset(grid2, 0, sizeof(grid2)); // memberi nilai 0

    // tanam bom secara acak
    for (int i = 1; i <= n; i++)
    {
        int a = rand() % n + 1;
        int b = rand() % n + 1;

        // memeriksa apakah kotak yang ingin ditanam bom sudah tertanam sebuah bom atau belum
        if (grid2[a][b] == -1)
        {
            i--; // agar i tidak bertambah di selanjutnya
            continue;
        }
        grid2[a][b] = -1;
    }

    // setelah bom ditentukan inisialisasi seluruh kotak dengan jumlah bom di sekitar
    inisialisasi_grid2();

    // cetak seluruh kotak
    cetak();

    while (true)
    {
        mainn(); //
        // memeriksa jika semua kotak yang bukan berisi bom sudah dibuka maka pemain menang
        if (jumlah_kotak_menang == 0)
        {
            menang();
            break;
        }
    }

    // menanyakan ke pemain apakah ingin bermain lagi dan pemain membrikan input y atau n
    char x;
    printf("Ingin bermain lagi? [y/n] : ");
    scanf(" %c", &x);

    // jika pemain ingin bermain lagi maka akan mengulangi fungsi ini dari awal
    // jika tidak maka program berahir
    if (x == 'y')
        main1();
}

int main()
{
    // inisialisasi serangkaian angka secara acak
    srand(time(NULL));

    // masuk ke fungsi game
    main1();

    // membersiihkan terminal ketika selesai bermain
    clear_screen();
}
