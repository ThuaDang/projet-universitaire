package sae;

import java.io.*;
import java.util.ArrayList;

public class Billetterie {
    private ArrayList<Client> clients = new ArrayList<>();

    public ArrayList<Client> getClients() {
        return clients;
    }

    public void ajouterClient(Client c) {
        clients.add(c);
    }

    public ArrayList<Client> rechercherParNom(String nom) {
        ArrayList<Client> res = new ArrayList<>();
        for (Client c : clients) {
            if (c.getNom().toLowerCase().contains(nom.toLowerCase())) res.add(c);
        }
        return res;
    }

    public ArrayList<Client> rechercherParPrenom(String prenom) {
        ArrayList<Client> res = new ArrayList<>();
        for (Client c : clients) {
            if (c.getPrenom().toLowerCase().contains(prenom.toLowerCase())) res.add(c);
        }
        return res;
    }

    public ArrayList<Client> rechercherParVille(String ville) {
        ArrayList<Client> res = new ArrayList<>();
        for (Client c : clients) {
            if (c.getAdresse().getVille().toLowerCase().contains(ville.toLowerCase())) res.add(c);
        }
        return res;
    }

    public String prochainIdentifiantClient() {
        int max = 0;
        for (Client c : clients) {
            try {
                int currentId = Integer.parseInt(c.getIdentifiant());
                if (currentId > max) max = currentId;
            } catch (NumberFormatException e) {
                // Ignore if it's not a number
            }
        }
        return String.valueOf(max + 1);
    }

    public int prochainNumeroBillet() {
        int max = 0;
        for (Client c : clients) {
            for (Billet b : c.getSesBillets()) {
                if (b.getNumero() > max) max = b.getNumero();
            }
        }
        return max + 1;
    }

    // Réutilise un spectacle existant de même nom et même type, sinon en crée un
    public Spectacle trouverOuCreerSpectacle(String type, String nom) {
        for (Client c : clients) {
            for (Billet b : c.getSesBillets()) {
                Spectacle s = b.getSpectacle();
                if (s.getNom().equals(nom) && s.getClass().getSimpleName().equals(type)) return s;
            }
        }
        if (type.equals("Orchestre")) return new Orchestre(nom);
        return new Artiste(nom);
    }

    public Salle trouverOuCreerSalle(String nom) {
        for (Client c : clients) {
            for (Billet b : c.getSesBillets()) {
                if (b.getSalle().getNom().equals(nom)) return b.getSalle();
            }
        }
        return new Salle(nom);
    }

    public void ajouterBillet(Client c, Billet b) {
        c.ajouter_Billet(b);
        b.getSalle().ajouter_billet(b);
        b.getSpectacle().ajouter_Billet(b);
    }

    // Spectacles distincts pour lesquels le client a des billets
    public ArrayList<Spectacle> spectaclesDuClient(Client c) {
        ArrayList<Spectacle> res = new ArrayList<>();
        for (Billet b : c.getSesBillets()) {
            if (!res.contains(b.getSpectacle())) res.add(b.getSpectacle());
        }
        return res;
    }

    public ArrayList<Billet> billetsDuSpectacle(Client c, Spectacle s) {
        ArrayList<Billet> res = new ArrayList<>();
        for (Billet b : c.getSesBillets()) {
            if (b.getSpectacle() == s) res.add(b);
        }
        return res;
    }

    public boolean payeParCarte(ArrayList<Billet> billets) {
        for (Billet b : billets) {
            if (b.getModeDePaiement() == ModePaiement.CARTE_BANCAIRE) return true;
        }
        return false;
    }

    // Espèce reste en espèce ; carte selon le choix du client
    public ModePaiement modeRemboursement(Billet b, ModePaiement choixSiCarte) {
        if (b.getModeDePaiement() == ModePaiement.ESPECE) return ModePaiement.ESPECE;
        return choixSiCarte;
    }

    public void rembourser(Client c, Spectacle s) throws IOException {
        ArrayList<Billet> billets = billetsDuSpectacle(c, s);
        for (Billet b : billets) {
            c.getSesBillets().remove(b);
            b.getSalle().getSesBillets().remove(b);
            b.getSpectacle().getSesBillets().remove(b);
        }
        sauvegarder();
    }

    public void sauvegarder() throws IOException {
        ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("donnees.ser"));
        out.writeObject(clients);
        out.close();
    }

    public void charger() {
        File f = new File("donnees.ser");
        if (!f.exists()) return;
        try {
            ObjectInputStream in = new ObjectInputStream(new FileInputStream(f));
            in.close();
        } catch (Exception e) {
            clients = new ArrayList<>();
        }
    }
}
