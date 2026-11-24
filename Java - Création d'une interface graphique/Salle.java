package sae;

import java.io.Serializable;
import java.util.HashSet;

public class Salle implements Serializable {
    private String nom;
    private HashSet<Billet> sesBillets = new HashSet<>();

    public Salle(String n){
        this.nom = n;
    }

    public void ajouter_billet(Billet b){
        this.sesBillets.add(b);
    }

    public void retirer_billet(Billet b){
        this.sesBillets.remove(b);
    }

    public String getNom() {
        return nom;
    }

    public HashSet<Billet> getSesBillets() {
        return sesBillets;
    }

    public String toString() {
        return nom;
    }
}
