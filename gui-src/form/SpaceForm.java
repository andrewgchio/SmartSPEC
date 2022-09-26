package form;

import java.util.stream.Collectors;

import javafx.event.ActionEvent;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import model.BackdropModel;
import model.Model;
import model.SpacesModel;
import model.SpacesModel.Space;
import view.View;
import viewutils.DoubleField;
import viewutils.IntField;

public class SpaceForm extends Form {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    protected IntField  id;
    protected TextField description;
    protected HBox      coordinates;
    protected IntField  capacity;
    protected Label     neighbors;

    private boolean isNew = true;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Construct a Space Form
     */
    public SpaceForm() {
        super();

        // id
        id = new IntField();
        addEntry("Space ID:", id);

        // description
        description = new TextField();
        addEntry("Description:", description);

        // coordinates
        coordinates = makeCoordinatesEntry();
        addEntry("Coordinates:", coordinates);

        // capacity
        capacity = new IntField();
        addEntry("Capacity:", capacity);

        // neighbors - added visually
        neighbors = new Label();
        addEntry("Neighbors:", neighbors);

        addSave(this::saveForm);
    }

    /**
     * Construct a Space Form using the provided space as a default
     * 
     * @param c A space
     */
    public SpaceForm(Space c) {
        this();

        // id will be grayed out and cannot be modified
        id.setValue(c.getID());
        id.setEditable(false);
        id.setStyle("-fx-control-inner-background: #D3D3D3;");

        // description
        description.setText(c.getDescription());

        // coordinates
        setCoordinates(c.getX(), c.getY(), c.getZ());

        // capacity
        capacity.setValue(c.getCapacity());

        // neighbors
        neighbors.setText(
                Model.getSpaces().get(c.getID()).getNeighbors().stream()
                .sorted()
                .map(x -> x.toString())
                .collect(Collectors.joining(",", "[", "]")));

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Form Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the HBox to input coordinates
     * 
     * @return The HBox to input coordinates
     */
    private HBox makeCoordinatesEntry() {
        return new HBox(
                new DoubleField(),  // x
                new DoubleField(),  // y
                new DoubleField()); // z
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the associated space ID
     * 
     * @return The associated space ID
     */
    public Integer getID() {
        return id.getValue();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Setters
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set the coordinates in the Space Form
     * 
     * @param x The x-coordinate to set
     * @param y The y-coordinate to set
     * @param z The z-coordinate to set
     */
    public void setCoordinates(double x, double y, double z) {
        ((DoubleField) coordinates.getChildren().get(0)).setValue(x);
        ((DoubleField) coordinates.getChildren().get(1)).setValue(y);
        ((DoubleField) coordinates.getChildren().get(2)).setValue(z);
    }

    /**
     * Flag to determine if space is new or edited
     * 
     * @param isNew Set to true if the space is new
     */
    public void setIsNew(boolean isNew) {
        this.isNew = isNew;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Save the input provided by the user into a space
     * 
     * @param e
     */
    private void saveForm(ActionEvent e) {
        idFail = !isNew && Model.getSpaces().containsID(id.getValue());
        if (idFail) {
            View.log("Error: space " + id.getValue() + " already exists");
            return;
        }

        SpacesModel cm = Model.getSpaces();
        Space       c  = cm.new Space();

        // id
        c.setID(id.getValue());
        
        // description
        c.setDescription(description.getText());

        // coordinates
        c.setCoordinates(
                ((DoubleField) coordinates.getChildren().get(0)).getValue(),
                ((DoubleField) coordinates.getChildren().get(1)).getValue(),
                ((DoubleField) coordinates.getChildren().get(2)).getValue());

        // floor
        BackdropModel bd = Model.getBackdrop();
        c.setFloor(bd.getFloorFromZ(c.getZ()));


        // capacity
        c.setCapacity(capacity.getValue());

        // neighbors will be added visually

        cm.add(c);

        View.log("Saved space (id=" + id.getValue() + ")");

    }

}
