package sae;

import java.io.Serializable;
import java.util.HashSet;

public class Adresse implements Serializable {
    private int numero;
    private String rue;
    private String ville;
    private String codePostal;
    private HashSet<Client> sesClients = new HashSet<>();

    public Adresse(int numero, String rue, String ville, String codePostal) {
        this.numero = numero;
        this.rue = rue;
        this.ville = ville;
        this.codePostal = codePostal;
    }

    public void ajouter_client(Client client){
        this.sesClients.add(client);
    }

    public void retirer_client(Client client){
        this.sesClients.remove(client);
    }

    public int getNumero() {
        return numero;
    }

    public String getRue() {
        return rue;
    }

    public String getVille() {
        return ville;
    }

    public String getCodePostal() {
        return codePostal;
    }

    public HashSet<Client> getSesClients() {
        return sesClients;
    }

    public String toString() {
        return numero + " " + rue + ", " + codePostal + " " + ville;
    }
}
