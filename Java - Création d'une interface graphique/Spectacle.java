package sae;

import java.io.Serializable;
import java.util.HashSet;

public abstract class Spectacle implements Serializable {
    private String nom;
    public HashSet<Billet> sesBillets = new HashSet<>();

    public Spectacle(String n){
        this.nom = n;
    }

    public void ajouter_Billet(Billet billet){
        this.sesBillets.add(billet);
    }

    public void retirer_Billet(Billet billet) throws MinimumBilletException{
        if (this.sesBillets.size() == 1){
            throw new MinimumBilletException("Un spectacle doit posséder au moins un billet.");
        } else {
            this.sesBillets.remove(billet);
        }
    }

    public String getNom() {
        return nom;
    }

    public HashSet<Billet> getSesBillets() {
        return sesBillets;
    }

    public String toString() {
        return getClass().getSimpleName() + " " + nom + " (" + sesBillets.size() + " billet(s))";
    }
}
