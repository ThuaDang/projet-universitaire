import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.stage.Stage;
import java.util.ArrayList;

public class FenetreRemboursement extends Stage {
    
    private Billetterie billetterie;
    private ComboBox<Client> choixClient = new ComboBox<>();
    private ListView<Spectacle> listeSpectacles = new ListView<>();
    private Label resultat = new Label();

    public FenetreRemboursement(Billetterie b) {
        billetterie = b;
        setTitle("Remboursement");
        choixClient.getItems().setAll(b.getClients());
        choixClient.valueProperty().addListener((o, a, c) -> afficherSpectacles(c));

        VBox racine = new VBox(10,
                new Label("Client"), choixClient,
                new Label("Spectacles du client"), listeSpectacles,
                boutonRembourser(), resultat);
        setScene(new Scene(racine, 500, 450));
    }

    private void afficherSpectacles(Client c) {
        listeSpectacles.getItems().clear();
        if (c != null) listeSpectacles.getItems().addAll(billetterie.spectaclesDuClient(c));
    }

    private Button boutonRembourser() {
        Button b = new Button("Rembourser ce spectacle");
        b.setOnAction(e -> rembourser());
        return b;
    }

    private void rembourser() {
        Client c = choixClient.getValue();
        Spectacle s = listeSpectacles.getSelectionModel().getSelectedItem();
        if (c == null || s == null) { resultat.setText("Choisir un client et un spectacle."); return; }

        ArrayList<Billet> billets = billetterie.billetsDuSpectacle(c, s);
        ModePaiement choix = choisirMode(billets);
        if (choix == null) return;

        try { billetterie.rembourser(c, s); } catch (Exception ex) {}
        resultat.setText(billets.size() + " billet(s) remboursé(s).");
        afficherSpectacles(c);
    }

    private ModePaiement choisirMode(ArrayList<Billet> billets) {
        if (!billetterie.payeParCarte(billets)) return ModePaiement.ESPECE;
        ArrayList<ModePaiement> options = new ArrayList<>();
        options.add(ModePaiement.ESPECE);
        options.add(ModePaiement.CARTE_BANCAIRE);
        ChoiceDialog<ModePaiement> d = new ChoiceDialog<>(ModePaiement.ESPECE, options);
        d.setHeaderText("Billets payés par carte : remboursement par ?");
        return d.showAndWait().orElse(null);
    }
}
