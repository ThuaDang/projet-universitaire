package sae;

import java.io.Serializable;
import java.util.HashSet;

public class Client implements Serializable {
    private String identifiant;
    private String nom;
    private String prenom;
    private String email;
    private Adresse adresse;
    private HashSet<Billet> sesBillets;

    public Client(String identifiant, String nom, String prenom, String email, Adresse adresse, HashSet<Billet> sesBillets) {
        this.identifiant = identifiant;
        this.nom = nom;
        this.prenom = prenom;
        this.email = email;
        this.adresse = adresse;
        this.sesBillets = sesBillets;
    }

    public void ajouter_Billet(Billet billet){
        this.sesBillets.add(billet);
    }

    public void retirer_Billet(Billet billet) throws MinimumBilletException{
        if (this.sesBillets.size() == 1){
            throw new MinimumBilletException("Un client doit posséder au moins un billet.");
        } else {
            this.sesBillets.remove(billet);
        }
    }

    public String getIdentifiant() {
        return identifiant;
    }

    public String getNom() {
        return nom;
    }

    public String getPrenom() {
        return prenom;
    }

    public String getEmail() {
        return email;
    }

    public Adresse getAdresse() {
        return adresse;
    }

    public HashSet<Billet> getSesBillets() {
        return sesBillets;
    }

    public String toString() {
        return prenom + " " + nom + " - " + email + " - " + adresse;
    }
}
