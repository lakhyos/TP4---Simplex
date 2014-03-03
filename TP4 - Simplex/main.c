//EL MIDAOUI Mohamed Ayoub
//BOUHJIRA Youssef

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>


#define M 20
#define N 20

static const double epsilon   = 1.0e-8;
int egual(double a, double b)
{
    return fabs(a-b) < epsilon;
}

typedef struct {
    int m, n; // m=lignes, n=colonnes, mat[m x n]
    double mat[M][N];
} Tableau;

void nl(int k)
{
    int j;
    for(j=0;j<k;j++) putchar('-');
    putchar('\n');
}

void print_tableau(Tableau *tab, const char* mes)
{
    int counter = 0;
    int i, j;
    printf("\n%d. Tableau %s:\n", ++counter, mes);
    nl(70);
    
    printf("%-6s%5s", "col:", "b[i]");
    for(j=1;j<tab->n; j++)
    {
        printf("    x%d,", j);
    }
    printf("\n");
    
    for(i=0;i<tab->m; i++)
    {
        if (i==0) printf("max:");
        else
            printf("b%d: ", i);
        for(j=0;j<tab->n; j++)
        {
            if (egual((int)tab->mat[i][j], tab->mat[i][j]))
                printf(" %6d", (int)tab->mat[i][j]);
            else
                printf(" %6.2lf", tab->mat[i][j]);
        }
        printf("\n");
    }
    nl(70);
}

/* Exemple de donnees a stocker dans nomfichier.txt read_tableau:
 4 5
 0   -0.5  -3 -1  -4
 40    1     1  1   1
 10   -2    -1  1   1
 10    0     1  0  -1
 */

void read_tableau(Tableau *tab, const char * nomfichier) {
    int err, i, j;
    FILE * fp;
    
    fp  = fopen(nomfichier, "r" );
    if( !fp )
    {
        printf("Ouverture de fichier impossible %s\n", nomfichier);
        exit(1);
    }
    memset(tab, 0, sizeof(*tab));
    err = fscanf(fp, "%d %d", &tab->m, &tab->n);
    if (err == 0 || err == EOF)
    {
        printf("Impossible de lire m ou n\n");
        exit(1);
    }
    for(i=0;i<tab->m; i++)
    {
        for(j=0;j<tab->n; j++)
        {
            err = fscanf(fp, "%lf", &tab->mat[i][j]);
            if (err == 0 || err == EOF)
            {
                printf("Impossible de lire A[%d][%d]\n", i, j);
                exit(1);
            }
        }
    }
    printf("Lire tableau [%d Lingnes x %d Colonnes] a partir du fichier"
           "'%s'.\n", tab->m, tab->n, nomfichier);
    fclose(fp);
}

void pivotage(Tableau *tab, int ligne, int col)
{
    int i, j;
    double pivot;
    
    pivot = tab->mat[ligne][col];
    assert(pivot>0);
    for(j=0; j<tab->n; j++)
        tab->mat[ligne][j] /= pivot;
    assert( egual(tab->mat[ligne][col], 1. ));
    
    for(i=0; i<tab->m; i++)
    { // Pour toute ligne restante i faire:
        double multiplier = tab->mat[i][col];
        if(i==ligne) continue;
        for(j=0; j<tab->n; j++)
        { // l[i] = l[i] - z * l[ligne];
            tab->mat[i][j] -= multiplier * tab->mat[ligne][j];
        }
    }
}

// Trouver pivot_col = la colonne la plus negatif dans mat[0][1..n]
int trouver_pivot_col(Tableau *tab)
{
    int j, pivot_col = 1;
    double minimal = tab->mat[0][pivot_col];
    for(j=1; j<tab->n; j++)
    {
        if (tab->mat[0][j] < minimal)
        {
            minimal = tab->mat[0][j];
            pivot_col = j;
        }
    }
    printf("La colonne la plus negatif dans row[0] est col %d = %g.\n",
           pivot_col, minimal);
    if( minimal >= 0 )
    {
        return -1; // Toutes le colonnes positifs dans row[0], c'est l'optimal.
    }
    return pivot_col;
}

// Trouver pivot_row, avec le plus petit ratio (= col[0] / col[pivot]) positive
int trouver_pivot_ligne(Tableau *tab, int pivot_col) {
    int i, pivot_ligne = 0;
    double min_ratio = -1;
    printf("Ratios A[ligne_i,0]/A[ligne_i,%d] = [", pivot_col);
    for(i=1;i<tab->m;i++)
    {
        double ratio = tab->mat[i][0] / tab->mat[i][pivot_col];
        printf("%3.2lf, ", ratio);
        if ( (ratio > 0  && ratio < min_ratio ) || min_ratio < 0 )
        {
            min_ratio = ratio;
            pivot_ligne = i;
        }
    }
    printf("].\n");
    if (min_ratio == -1)
        return -1; // Non liee.
    printf("Pivot A[%d,%d] trouve, min positive ratio=%g a ligne=%d.\n",
           pivot_ligne, pivot_col, min_ratio, pivot_ligne);
    return pivot_ligne;
}

void ajout_variables_ecart(Tableau *tab)
{
    int i, j;
    for(i=1; i<tab->m; i++)
    {
        for(j=1; j<tab->m; j++)
            tab->mat[i][j + tab->n -1] = (i==j);
    }
    tab->n += tab->m -1;
}

void verif_b_positive(Tableau *tab)
{
    int i;
    for(i=1; i<tab->m; i++)
        assert(tab->mat[i][0] >= 0);
}

/* Etant donnees le colonnes de la matrice identite, trouvant la ligne
   contenant 1.*/
// return -1, si la colonne n'est pas celle d'une matrice indentite.
int touver_variable_basic(Tableau *tab, int col) {
    int i, xi=-1;
    for(i=1; i < tab->m; i++)
    {
        if (egual( tab->mat[i][col],1) )
        {
            if (xi == -1)
                xi=i;   // '1' trouve, sauvegarder le num de la ligne.
            else
                return -1; // second '1' trouve, ce n'est une matrice identite.
            
        }
        else if (!egual( tab->mat[i][col],0) )
        {
            return -1; // pas une colonne de matrice identite.
        }
    }
    return xi;
}

void afficher_vecteur_optimal(Tableau *tab, char *message) {
    int j, xi;
    printf("%s a ", message);
    for(j=1;j<tab->n;j++) { // pour chaque colonne.
        xi = touver_variable_basic(tab, j);
        if (xi != -1)
            printf("x%d=%3.2lf, ", j, tab->mat[xi][0] );
        else
            printf("x%d=0, ", j);
    }
    printf("\n");
}

void simplex(Tableau *tab)
{
    int boucle=0;
    ajout_variables_ecart(tab);
    verif_b_positive(tab);
    print_tableau(tab,"Variables d'ecart ajoutees");
    while( ++boucle )
    {
        int pivot_col, pivot_ligne;
        
        pivot_col = trouver_pivot_col(tab);
        if( pivot_col < 0 ) {
            printf("Valeur Optimal trouvee V=A[0,0]=%3.2lf (pas de nombres negatives dans la ligne 0).\n",
                   tab->mat[0][0]);
            afficher_vecteur_optimal(tab, "Vecteur Optimal");
            break;
        }
        printf("La variable entrante est x%d, alors pivot_col=%d.\n",
               pivot_col, pivot_col);
        
        pivot_ligne = trouver_pivot_ligne(tab, pivot_col);
        if (pivot_ligne < 0)
        {
            printf("non liee (pas de pivot_ligne).\n");
            break;
        }
        printf("La variable sortante est x%d, alors pivot_ligne=%d\n", pivot_ligne, pivot_ligne);
        
        pivotage(tab, pivot_ligne, pivot_col);
        print_tableau(tab,"Apres pivotage");
        afficher_vecteur_optimal(tab, "Solution basique realisable");
        
        if(boucle > 20)
        {
            printf("Beaucoup trop d'iterations > %d.\n", boucle);
            break;
        }
    }
}


//Exemple de systeme (probleme)
Tableau tab  = { 4, 5, {                     // Taille du tableau [4 lignes x 5 colonnes ]
    {  0.0 , -0.5 , -3.0 ,-1.0 , -4.0,   },  // Max: z = 0.5*x + 3*y + z + 4*w,
    { 40.0 ,  1.0 ,  1.0 , 1.0 ,  1.0,   },  //    x + y + z + w <= 40 .. b1
    { 10.0 , -2.0 , -1.0 , 1.0 ,  1.0,   },  //  -2x - y + z + w <= 10 .. b2
    { 10.0 ,  0.0 ,  1.0 , 0.0 , -1.0,   },  //        y     - w <= 10 .. b3
}
};

int main(int argc, char *argv[])
{
    // Tableau *tab;
    if (argc > 1)
    {
        read_tableau(&tab, argv[1]);
    }
    print_tableau(&tab,"Initial");
    simplex(&tab);
    return 0;
}
