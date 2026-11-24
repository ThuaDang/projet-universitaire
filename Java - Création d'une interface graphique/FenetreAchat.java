import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.Stage;
import java.util.HashSet;

public class FenetreAchat extends Stage {

    // Elements de la fenêtre
    private Billetterie billetterie;
    private ListView<Client> listeClients = new ListView<>();
    private Label statut = new Label();
    
    // Création d'un client
    private TextField nomF = new TextField();
    private TextField prenomF = new TextField();
    private TextField emailF = new TextField();
    private TextField numeroF = new TextField();
    private TextField rueF = new TextField();
    private TextField villeF = new TextField();
    private TextField cpF = new TextField();

    // Création d'un billet
    private ComboBox<String> typeF = new ComboBox<>();
    private TextField artisteF = new TextField();
    private TextField salleF = new TextField();
    private DatePicker dateF = new DatePicker();
    private TextField heureF = new TextField("20:30");
    private ComboBox<ModePaiement> paiementF = new ComboBox<>();
    

    public FenetreAchat(Billetterie b) {
        billetterie = b;
        setTitle("Achat de billets");
        listeClients.setPrefHeight(120);
        

        VBox racine = new VBox(10,
                new Label("Créer un client"), formClient(), boutonCreer(),
                new Label("Clients"), barreRecherche(), listeClients,
                new Label("Acheter un billet pour le client sélectionné"), formBillet(), boutonAcheter(), statut,
                boutonRemboursement());

        rafraichirClients();
        setScene(new Scene(racine, 700, 720));
    }

    // Formulaire de création d'un client
    private VBox formClient() {
        nomF.setPromptText("Nom");
        prenomF.setPromptText("Prénom");
        emailF.setPromptText("Email");
        numeroF.setPromptText("N°");
        rueF.setPromptText("Rue");
        villeF.setPromptText("Ville");
        cpF.setPromptText("Code postal");
        return new VBox(5, nomF, prenomF, emailF, numeroF, rueF, villeF, cpF);
    }

    // Bouton pour créer un client
    private Button boutonCreer() {
        Button b = new Button("Créer le client");
        b.setOnAction(e -> creerClient());
        return b;
    }

    //Création d'un client à partir des champs du formulaire
    private void creerClient() {
        if (nomF.getText().isEmpty() || prenomF.getText().isEmpty()) {
            statut.setText("Nom et prénom obligatoires.");
            return;
        }
        int num = 0;
        try { num = Integer.parseInt(numeroF.getText()); } catch (Exception ex) {}
        Adresse a = new Adresse(num, rueF.getText(), villeF.getText(), cpF.getText());
        Client c = new Client(nomF.getText(), prenomF.getText(), emailF.getText(), a, new HashSet<>());
        billetterie.ajouterClient(c);
        rafraichirClients();
        statut.setText("Client créé.");
    }

    // Barre de recherche pour filtrer les clients
    private HBox barreRecherche() {
        ComboBox<String> critere = new ComboBox<>();
        critere.getItems().addAll("Tous", "Nom", "Ville");
        critere.setValue("Tous");
        TextField valeur = new TextField();
        Button b = new Button("Rechercher");
        b.setOnAction(e -> chercher(critere.getValue(), valeur.getText()));
        return new HBox(8, critere, valeur, b);
    }

    // Méthode pour filtrer les clients selon le critère choisi
    private void chercher(String critere, String v) {
        if (critere.equals("Nom")) listeClients.getItems().setAll(billetterie.rechercherParNom(v));
        else if (critere.equals("Ville")) listeClients.getItems().setAll(billetterie.rechercherParVille(v));
        else listeClients.getItems().setAll(billetterie.getClients());
    }

    // Formulaire pour acheter un billet
    private VBox formBillet() {
        typeF.getItems().addAll("Artiste", "Orchestre");
        typeF.setValue("Artiste");
        artisteF.setPromptText("Nom artiste/orchestre");
        salleF.setPromptText("Salle");
        paiementF.getItems().addAll(ModePaiement.values());
        paiementF.setValue(ModePaiement.CARTE_BANCAIRE);
        return new VBox(5, typeF, artisteF, salleF, dateF, heureF, paiementF);
    }

    // Bouton pour acheter un billet
    private Button boutonAcheter() {
        Button b = new Button("Acheter le billet");
        b.setOnAction(e -> acheterBillet());
        return b;
    }

    // Méthode pour acheter un billet
    private void acheterBillet() {
        Client c = listeClients.getSelectionModel().getSelectedItem();
        if (c == null) { statut.setText("Sélectionnez un client."); return; }
        if (artisteF.getText().isEmpty() || salleF.getText().isEmpty() || dateF.getValue() == null) {
            statut.setText("Remplissez artiste, salle et date.");
            return;
        }
        Spectacle s = billetterie.trouverOuCreerSpectacle(typeF.getValue(), artisteF.getText());
        Salle sa = billetterie.trouverOuCreerSalle(salleF.getText());
        Billet b = new Billet(billetterie.prochainNumeroBillet(), java.sql.Date.valueOf(dateF.getValue()),
                paiementF.getValue(), heureF.getText(), c, sa, s);
        billetterie.ajouterBillet(c, b);
        statut.setText("Billet acheté pour " + c.getPrenom() + ".");
    }

    // Bouton pour aller à la fenêtre de remboursement
    private Button boutonRemboursement() {
        Button b = new Button("Aller au remboursement");
        b.setOnAction(e -> new FenetreRemboursement(billetterie).show());
        return b;
    }

    // Méthode pour rafraîchir la liste des clients
    private void rafraichirClients() {
        listeClients.getItems().setAll(billetterie.getClients());
    }
}
