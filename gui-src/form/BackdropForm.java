package form;

import java.io.File;

import javafx.event.ActionEvent;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;
import javafx.stage.Window;
import model.BackdropModel;
import model.BackdropModel.Backdrop;
import model.Model;
import modelutils.CoordinateManager;
import view.View;
import viewutils.DoubleField;

public class BackdropForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected TextField   floor;
    protected DoubleField z;

    protected VBox backdropEntry;

    protected HBox p0; // top left
    protected HBox p1; // bottom right

    public static final int DIR_WIDTH = 400;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Backdrop Form
     */
    public BackdropForm() {
        super();

        // Select floor
        floor = new TextField();
        addEntry("Select floor:", floor);

        // Select z
        z = new DoubleField();
        addEntry("Z coordinate:", z);

        // Select image file
        backdropEntry = makeBackdropEntry();
        addEntry("Select Image:", backdropEntry);

        // Coordinates for image file
        addEntry("Image Coordinates:");

        p0 = makeXYEntry(0, 0);
        addSubEntry("Top Left:", p0);

        p1 = makeXYEntry(1, 1);
        addSubEntry("Bottom Right:", p1);

        addSave(this::saveForm);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Create an entry to select a backdrop file
     * 
     * @return An entry to select a backdrop file
     */
    private VBox makeBackdropEntry() {
        Label selection = new Label("None Selected");
        selection.setWrapText(true);
        selection.setMaxWidth(DIR_WIDTH);

        Button selectButton = new Button("Select");
        selectButton.setOnAction(e -> {
            FileChooser chooser  = new FileChooser();
            Window      w        = selectButton.getParent().getScene().getWindow();
            File        selected = chooser.showOpenDialog(w);
            if (selected != null)
                selection.setText(selected.getAbsolutePath());
        });

        return new VBox(selection, selectButton);
    }

    /**
     * Make an xy coordinate entry box
     * @param initX The initial x-coordinate
     * @param initY The initial y-coordinate
     * @return An xy coordinate entry box
     */
    private HBox makeXYEntry(double initX, double initY) {
        return new HBox(new DoubleField(initX), new DoubleField(initY));
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a backdrop
     * @param e
     */
    private void saveForm(ActionEvent e) {
        BackdropModel bm = Model.getBackdrop();
        if (bm.streamFloors().anyMatch(x -> x.equals(floor.getText()))) {
            View.log("Error: floor " + floor.getText() + " already exists");
            return;
        }

        Backdrop      bd = bm.new Backdrop();

        // floor
        bd.setFloor(floor.getText());

        // z coordinate
        bd.setZ((double) z.getValue());

        // image path
        String fimage = ((Label) backdropEntry.getChildren().get(0)).getText();
        bd.setImage(fimage);

        // coordinates
        CoordinateManager coords = bd.getCoordinateManager();
        coords.x0 = ((DoubleField) p0.getChildren().get(0)).getValue();
        coords.y0 = ((DoubleField) p0.getChildren().get(1)).getValue();
        coords.x1 = ((DoubleField) p1.getChildren().get(0)).getValue();
        coords.y1 = ((DoubleField) p1.getChildren().get(1)).getValue();

        bm.addSpaceBackdrop(bd);

        bm.writeToFile();

        View.getInstance().getTabs().addTab(bd);
    }

}
