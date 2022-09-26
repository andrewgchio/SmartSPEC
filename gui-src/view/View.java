package view;

import java.io.File;

import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.MenuBar;
import javafx.scene.layout.Priority;
import javafx.scene.layout.VBox;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

/**
 * The View class in the MVC pattern.
 */
public class View {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of View class
     */
    private static final View instance = new View();

    /**
     * Return an instance of the View class
     *
     * @return An instance of the View class
     */
    public static View getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private View() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // GUI Content
    ////////////////////////////////////////////////////////////////////////////

    /**
     * JavaFX primary stage
     */
    private Stage primaryStage;

    /**
     * JavaFX scene
     */
    private Scene scene;

    /**
     * Main content to display
     */
    private VBox pane = new VBox();

    /**
     * Top menu bar
     */
    private TopMenu menu;

    /**
     * Tabs for each space backdrop / floor
     */
    private SpacesTabPane tabs;

    /**
     * Status line to display at the bottom of the space tab
     */
    private StatusLabel status;

    /**
     * Output pane to write logs
     */
    private OutputPane output;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Margin to place between nodes
     */
    public static final int MARGIN = 10;

    /**
     * Width of the application window
     */
    public static final int WINDOW_WIDTH = 1000;

    /**
     * Height of the application window
     */
    public static final int WINDOW_HEIGHT = 800;

    /**
     * Percentage of window to display space tabs
     */
    public static final double TAB_OUTPUT_SPLIT = 0.8;

//    	public static final int TP_WIDTH = 300;
//    	public static final int TP_HEIGHT = 100;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // GUI Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Build a scene and set the stage
     */
    public void build() {
        // Initialize scene
        scene = new Scene(pane, WINDOW_WIDTH, WINDOW_HEIGHT);
        primaryStage.setScene(scene);
        primaryStage.setTitle("SmartSPEC GUI");

        // Top menu bar
        menu = new TopMenu();

        // Spaces
        tabs = new SpacesTabPane();
        tabs.setPrefHeight(scene.getHeight() * TAB_OUTPUT_SPLIT);
        VBox.setVgrow(tabs, Priority.ALWAYS);

        // Spaces status
        status = new StatusLabel();

        // Output Pane
        output = new OutputPane();
        output.setPrefHeight(scene.getHeight() * (1 - TAB_OUTPUT_SPLIT));

        // Add GUI components
        pane.getChildren().addAll(menu, tabs, status, output);
    }

    /**
     * Show the stage
     */
    public void show() {
        primaryStage.show();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the JavaFX primary stage
     *
     * @return The JavaFX primary stage
     */
    public Stage getPrimaryStage() {
        return primaryStage;
    }

    /**
     * Return the top menu bar
     *
     * @return The top menu bar
     */
    public MenuBar getMenu() {
        return menu;
    }

    /**
     * Return the spaces tab pane
     *
     * @return The spaces tab pane
     */
    public SpacesTabPane getTabs() {
        return tabs;
    }

    /**
     * Return the space status
     *
     * @return The space status
     */
    public Label getStatus() {
        return status;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set a reference to the primary stage
     *
     * @param primaryStage The JavaFX primary stage
     */
    public void setPrimaryStage(Stage primaryStage) {
        this.primaryStage = primaryStage;
    }

    /**
     * Set the space tab status
     *
     * @param text The status text to display
     */
    public static void setStatus(String text) {
        View.getInstance().status.setStatus(text);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Prompt the user for a directory
     *
     * @return The selected file representing a directory, or null
     */
    public File promptDirectory() {
        return new DirectoryChooser().showDialog(primaryStage);
    }

    /**
     * Prompt the user for a file
     *
     * @return The selected file, or null
     */
    public File promptFile() {
        return new FileChooser().showOpenDialog(primaryStage);
    }

    /**
     * Create an entry in the view's log with the given text
     *
     * @param text The content to display
     */
    public static void log(String text) {
        View.getInstance().output.log(text);
    }
    
}
