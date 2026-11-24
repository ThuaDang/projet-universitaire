package sae;

import java.io.Serializable;
import java.util.Date;

public class Billet implements Serializable {
    private int numero;
    private Date date;
    private ModePaiement modeDePaiement;
    private String heureDebut;
    private Client client;
    private Salle salle;
    private Spectacle spectacle;

    public Billet(int numero, Date date, ModePaiement modeDePaiement, String heureDebut,
                  Client client, Salle salle, Spectacle spectacle) {
        this.numero = numero;
        this.date = date;
        this.modeDePaiement = modeDePaiement;
        this.heureDebut = heureDebut;
        this.client = client;
        this.salle = salle;
        this.spectacle = spectacle;
    }

    public int getNumero() {
        return numero;
    }

    public Date getDate() {
        return date;
    }

    public ModePaiement getModeDePaiement() {
        return modeDePaiement;
    }

    public String getHeureDebut() {
        return heureDebut;
    }

    public Client getClient() {
        return client;
    }

    public Salle getSalle() {
        return salle;
    }

    public Spectacle getSpectacle() {
        return spectacle;
    }

    public String toString() {
        return "Billet " + numero + " - " + spectacle.getNom() + " - " + salle.getNom()
             + " - " + date + " " + heureDebut + " - " + modeDePaiement;
    }
}
