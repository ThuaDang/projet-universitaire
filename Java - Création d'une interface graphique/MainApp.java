import javafx.application.Application;
import javafx.stage.Stage;


public class MainApp extends Application {
    private Billetterie billetterie = new Billetterie();

    public void init() throws Exception {
        billetterie.charger();
        
    }

    public void start(Stage stage) {
        FenetreAchat fenetreAchat = new FenetreAchat(billetterie);
        fenetreAchat.setWidth(1440);
        fenetreAchat.setHeight(1440);
        fenetreAchat.show();
    }

    public void stop() throws Exception {
        billetterie.sauvegarder();
    }



    public static void main(String[] args) {
        launch(args);
    }
}
