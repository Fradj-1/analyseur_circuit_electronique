#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circuit.h"


/* ============================
*  déclaration fonctions privée
* ============================ */


// Allocation de mémoire pour une matrice double
static double **alloc_matrice(int taille) ;

//Allocation de mémoire d'un vecteur double
static double *alloc_vecteur (int taille) ;

//  stamp résistance : matrix anm
static void stamp_resistance (double **anm_matrice, int noeud1, int noeud2, double resistance_valeur) ;

//stamp source de courant : I vecteur
static void stamp_courant (double *vecteur_source, int noeud1, int noeud2, double i_valeur) ;

// stamp source de tension : analyse modal modifiée (ANM)
static void stamp_voltage(double **anm_matrice, double *vecteur_source, int nombre_noeuds, int eq_index, int noeud1, int noeud2, double v_valeur) ;

/* ============================
*  déclaration fonctions publiques
* ============================ */

int build_anm_dc(const t_circuit *circuit, double ***anm_matrice_construite, double **vecteur_source_construit) ;
//  ***********************************
//  Definitions des fonctions privées
//  ***********************************

static double **alloc_matrice(int taille)
{
    double **matrice = calloc(taille,sizeof(double*));
    if (!matrice)
    {
        fprintf(stderr,"Erreur :  échec calloc ligne matrice\n") ;
        return NULL ;
    }
    for (int i=0 ; i<taille ; i++)
    {
        matrice[i] = calloc(taille,sizeof(double)) ;
        if (!matrice[i])
        {
            fprintf(stderr,"Erreur : échec calloc colonne matrice\n") ;
            // libérer l'espace
            for (int j=0 ; j<i ; j++)
            {
                free(matrice[j]) ;
                free(matrice) ;
                return NULL ;
            }

        }
    }
    return matrice ;
}

static double *alloc_vecteur (int taille)
{
    double *vecteur = calloc(taille,sizeof(double)) ;
    if (!vecteur)
    {
        fprintf(stderr,"Erreur : échec calloc  vecteur\n") ;
        return NULL ;
    }
    return vecteur ;
}


static void stamp_resistance (double **anm_matrice, int noeud1, int noeud2, double resistance_valeur)
{
    double g = 1.0/ resistance_valeur ;
    if (noeud1) anm_matrice[noeud1-1][noeud1-1] +=g ;
    if (noeud2) anm_matrice[noeud2-1][noeud2-1] +=g ;

    if (noeud1 && noeud2)
    {
        anm_matrice[noeud1-1][noeud2-1] -=g ;
        anm_matrice[noeud2-1][noeud1-1] -=g ;
    }
}

static void stamp_courant (double *vecteur_source, int noeud1, int noeud2, double i_valeur)
{
    if (noeud1) vecteur_source[noeud1-1] -=i_valeur ;
    if (noeud2) vecteur_source[noeud2-1] +=i_valeur ;
}

static void stamp_voltage(double **anm_matrice, double *vecteur_source, int nombre_noeuds, int eq_index, int noeud1, int noeud2, double v_valeur)
{
    int ligne = nombre_noeuds+ eq_index ;

    // Équation de la source de tension
    if (noeud1)
    {
        anm_matrice[ligne][noeud1-1] = 1.0 ;
        anm_matrice[noeud1-1][ligne] = 1.0 ;
    }
    if (noeud2)
    {
        anm_matrice[ligne][noeud2-1] = -1.0 ;
        anm_matrice[noeud2-1][ligne] = -1.0 ;
    }
    vecteur_source[ligne] = v_valeur ;
}


//  ***********************************
//  Definitions des fonctions publiques
//  ***********************************

int build_anm_dc(const t_circuit *circuit, double ***anm_matrice_construite, double **vecteur_source_construit)
{
    if (!circuit)
    {
        fprintf(stderr, "Erreur : circuit Null\n") ;
        return -1 ;
    }
    int nb_noeuds = circuit->nombre_noeud ;

    // Compter les sources de tension
    int nombre_vsources = 0 ;
    for (int i = 0 ; i< circuit->nombre_composant; i++)
    {
        if (circuit->nombre_composant[i].type == COMPOSANT_VSOURCE)
            nombre_vsources++ ;
    }
    int taille = nb_noeuds + nombre_vsources ;

    //Allocation
    double **anm_matrice = alloc_matrice(taille);
    if (!anm_matrice)
        return -1 ;

    double *vecteur_source = alloc_vecteur(taille);
    if (!vecteur_source)
    {
        for (int i = 0 ; i< taille ; i++)
        {
            free(anm_matrice[i]) ;
            free(anm_matrice) ;
            return -1 ;
        }
    }


        // Remplir matrice
        int v_index = 0 ;
        for (int i =0 ; i < circuit->nombre_composant; i++)
        {
            t_composant c = circuit->nombre_composant[i] ;

            switch (c.type)
            {
            case COMPOSANT_RESISTANCE : stamp_resistance(anm_matrice, c.noeud1, c.noeud2, c.valeur) ;
                break ;
            case COMPOSANT_ISOURCE : stamp_courant( vecteur_source, c.noeud1, c.noeud2, c.valeur) ;
                break ;
            case COMPOSANT_VSOURCE : stamp_voltage(anm_matrice, vecteur_source, nb_noeuds, v_index,c.noeud1,c.noeud2, c.valeur) ;
            v_index++ ;
                break ;

                default : break ;

            }
        }
    *anm_matrice_construite = anm_matrice ;
    *vecteur_source_construit = vecteur_source;

    return taille ;

    }

