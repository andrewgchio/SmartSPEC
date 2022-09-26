package form;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.VPos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import view.View;

public class Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    // Form contents
    protected Label title = null;
    protected VBox  form  = null;

    // Form layout
    private GridPane grid = null;
    private int      col  = 1;
    private int      row  = -1;

    // A save button to add at the end of the form
    protected Button save = null;

    // Set to true if the form was closed without saving
    protected boolean failed = false;
    protected boolean idFail = false;

    // Form Constants
    public static final int    GRID_MARGIN  = 10;
    public static final Insets GRID_PADDING = new Insets(0, 10, 10, 10);
    
    // For use within subclasses
    public static final int SPACING = 3;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Form object
     */
    public Form() {

        // Initialize form contents
        title = new Label();
        form  = new VBox();

        // Set up GridPane layout
        grid = new GridPane();
        grid.setHgap(GRID_MARGIN);
        grid.setVgap(GRID_MARGIN);
        grid.setPadding(GRID_PADDING);

        // Add relevant nodes to form
        form.getChildren().addAll(title, grid);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the form contents
     * 
     * @return The form contents
     */
    public VBox getForm() {
        return form;
    }

    /**
     * Return the name of the derived class (without "Form")
     * 
     * @return The name of the derived class (without "Form")
     */
    public String getName() {
        String classname = this.getClass().getSimpleName();
        return classname.substring(0, classname.length() - 4);
    }

    /**
     * Return true if the form was closed improperly
     * 
     * @return True if the form was closed improperly
     */
    public boolean isFailed() {
        return failed;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setter Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the title of the form
     * 
     * @param title The title of the form
     */
    public void setTitle(String title) {
        this.title.setText(title);
    }

    /**
     * Add a node entry into the form
     * 
     * @param entryTitle A title for the entry
     * @param node       The node to add
     */
    public void addEntry(String entryTitle, Node node) {

        // Initialize new Label
        Label label = new Label(entryTitle);
        GridPane.setValignment(label, VPos.TOP);
        grid.add(label, col, ++row);

        // Let VBox nodes span over 2 columns
        if (node instanceof VBox) {
            grid.add(node, col + 1, row, 2, 1);
        } 
        else if (node != null) {
            grid.add(node, col + 1, row);
        }
    }

    /**
     * Add a title into the form
     * 
     * @param entryTitle A title
     */
    public void addEntry(String entryTitle) {
        addEntry(entryTitle, null);
    }

    /**
     * Add a subentry (indented by 4 spaces) into the form
     * 
     * @param entryTitle The title of the subentry
     * @param node       The node to add
     */
    public void addSubEntry(String entryTitle, Node node) {
        addEntry("    " + entryTitle, node);
    }

    /**
     * Add a save button to the end of the form
     * 
     * @param saveAction The action to take when the save button is clicked
     */
    public void addSave(EventHandler<ActionEvent> saveAction) {

        // Initialize new Button
        save = new Button("Save");

        // When button is clicked, save contents of form
        save.setOnAction(e -> {
            try {
                saveAction.handle(e);
                if (!idFail) {
                    Stage stage = (Stage) save.getScene().getWindow();
                    stage.close();
                }
            } 
            catch (IllegalArgumentException e2) {
                View.log("Error: " + e2.getMessage());
            }
        });

        // Add save button to grid
        grid.add(save, col, ++row);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Display the form
     */
    public void show() {
        Stage stage = new Stage();
        stage.setTitle("Add/Edit " + getName());
        stage.setScene(new Scene(form));
        stage.setOnCloseRequest(e -> { failed = true; });
        stage.showAndWait();
    }

}
