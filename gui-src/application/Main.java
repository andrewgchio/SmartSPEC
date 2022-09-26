package application;

import javafx.application.Application;
import javafx.stage.Stage;
import view.View;

public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception {
        View view = View.getInstance();
        view.setPrimaryStage(primaryStage);
        view.build();
        view.show();

    }

    public static void main(String[] args) {
        Application.launch(args);
    }

}
