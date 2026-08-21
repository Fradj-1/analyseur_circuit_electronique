#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ============================
*  déclaration fonctions privée
* ============================ */

// allocation de mémoire pour le vecteur RHS (taille dépend du nombre de noeuds et de source de tension)
static double *alloc_vecteur(int taille);

// échange de deux lignes dans la matrice et le vecteur (utilisation pour éviter les risque de l'algorithme de Gauss
static void echange_rangee(double **anm_matrice, double *vecteur_second_membre, int ligne1, int ligne2, int taille ) ;

// recherche du plus grand pivot en valeur absolue
static int calcule_pivot(double **anm_matrice,int colonne, int taille) ;
/* ============================
*  déclaration fonctions publiques
* ============================ */
// Solveur gauss avec pivot partiel : Résout A*x = b
int resoudre_gauss( double **anm_matrice, double *vecteur_second_membre, int taille, double, double **vecteur_solution) ;

//  ***********************************
//  Definitions des fonctions privées
//  ***********************************

static double *alloc_vecteur(int taille)
{
    double *vecteur =calloc(taille,sizeof(double));
    if (!vecteur)
    {
        fprintf(stderr,"ERREUR: alloc_vecteur() échec calloc\n");
    }
    return vecteur ;
}

static void echange_rangee(double **anm_matrice, double *vecteur_second_membre, int ligne1, int ligne2, int taille )
{
    if (ligne1 == ligne2)
    {
        return;
    }


    // échanges des lignes de A
    for (int i = 0 ; i < taille ; i++)
    {
        double tmp  = anm_matrice[ligne1][i] ;
        anm_matrice[ligne1][i] = anm_matrice[ligne2][i] ;
        anm_matrice[ligne2][i] = tmp ;
    }
        // échange éléments de b
        double tmp2 = vecteur_second_membre[ligne1] ;
        vecteur_second_membre[ligne1] = vecteur_second_membre[ligne2] ;
        vecteur_second_membre[ligne2] = tmp2 ;

}

static int calcule_pivot(double **anm_matrice,int colonne, int taille)
{
    int ligne_pivot = colonne ;
    double valeur_max = fabs(anm_matrice[colonne][colonne]) ;

    for (int i = colonne+ 1 ; i < taille ; i++)
    {
        double valeur = fabs(anm_matrice[i][colonne]) ;
        if (valeur > valeur_max)
        {
            valeur_max = valeur ;
            ligne_pivot = i ;
        }
    }
    return ligne_pivot ;
}

//  ***********************************
//  Definitions des fonctions publiques
//  ***********************************

int resoudre_gauss( double **anm_matrice, double *vecteur_second_membre, int taille, double, double **vecteur_solution_sortie)
{
    if (!anm_matrice || !vecteur_second_membre || taille <= 0)
    {
        fprintf(stderr,"ERREUR: resoudre_gauss() a des arguments invalides\n");
        return -1 ;
    }
    // allocation vecteur solution
    double *vecteur_solution = alloc_vecteur(taille);
    if (!vecteur_solution)
    {
        return -1 ;
    }


    // élimination avant
    for (int i = 0 ; i< taille ; i++)
    { // pivot partiel
        int ligne_pivot = calcule_pivot(anm_matrice,i,taille);
        if (fabs(anm_matrice[ligne_pivot][i]) < 1e-15)
        {
            fprintf(stderr, "Erreur : matrice singulière (pivot environ 0)\n") ;
            free(vecteur_solution) ;
            return -1 ;
        }
        echange_rangee(anm_matrice, vecteur_second_membre,i, ligne_pivot, taille) ;

        // élimination sur les lignes suivantes
        for (int k = i +1 ; k < taille ; k++)
        {
            double facteur = anm_matrice[k][i] / anm_matrice[k][k] ;
            anm_matrice[k][i] = 0.0 ;

            for (int j = k+1 ; j < taille ; j++)
            {
                anm_matrice[k][j] -= facteur * anm_matrice[i][j] ;
            }
            vecteur_second_membre[k] -= facteur * vecteur_second_membre[i] ;
        }
    }
    // remontée arrière
    for (int i = taille -1; i >= 0 ; i--)
    {
        double somme = vecteur_second_membre[i] ;
        for (int j = i+1 ; j < taille ; j++)
        {
            somme -= anm_matrice[i][j] * vecteur_solution[j] ;
        }
        if ( fabs(anm_matrice[i][i]) < 1e-15 )
        {
            fprintf(stderr, "Erreur : division par pivot environ 0 en remontée\n") ;
            free(vecteur_solution) ;
            return -1 ;
        }
        vecteur_solution[i] = somme / anm_matrice[i][i] ;
    }
    *vecteur_solution_sortie = vecteur_solution ;
    return  0;

}