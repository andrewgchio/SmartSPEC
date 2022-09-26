package view;

import form.EventForm;
import form.Form;
import form.SensorForm;
import form.SpaceForm;
import javafx.scene.control.Button;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.AnchorPane;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Polygon;
import javafx.util.Duration;
import model.BackdropModel.Backdrop;
import model.Model;
import model.SpacesModel;
import model.SpacesModel.Space;
import modelutils.CoordinateManager;
import viewutils.IDChoiceBox;

/**
 * A SpaceNode is used to visualize a specific space in the GUI.
 */
public class SpaceNode extends Button {

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ///////////////////////////////////////////////////////////////////////////

    // id of corresponding space
    private Integer cid;

    // Variables to track dragging
    public static Space dragSpace = null;

    // Reference to corresponding space tab
    private SpaceTab tab;

    // Radius of nodes
    public final static int R = 25;

    // A regular circle
    public final static Circle R_CIRCLE = new Circle(R);

    // A triangle pointing up (to indicate a pathway to an upper floor)
    public final static Polygon UP_POLY = new Polygon(0, 0, R, 0, 0.5 * R, -R);

    // A triangle pointing down (to indicate a pathway to a lower floor)
    public final static Polygon DOWN_POLY = new Polygon(0, 0, R, 0, 0.5 * R, R);

    // A diamond (to indicate pathways going to upper and lower floors)
    public final static Polygon UP_DOWN_POLY = 
            new Polygon(0, 0, 0.5 * R, -0.5 * R, 0, -R, -0.5 * R, -0.5 * R);

    // Number of seconds to display tooltip
    private final static double TOOLTIP_DURATION = 0.1;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constructors / Initializers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Constructs a default SpaceNode. The updateWithSpace() method should be
     * called later.
     */
    public SpaceNode() {
        super();

        this.setShape(R_CIRCLE);
        this.setPrefSize(R, R);
        this.setStyle("-fx-background-color: #FFB6C1"); // Pink
    }

    /**
     * Constructs a SpaceNode based on the given space.
     * 
     * @param c   The space to display
     * @param tab A reference to the enclosing space tab
     */
    public SpaceNode(Space c, SpaceTab tab) {
        this(); // call default constructor
        updateWithSpace(c, tab);
    }

    /**
     * Updates the space node with the given space and enclosing space tab
     * 
     * @param c   The space to display
     * @param tab A reference to the enclosing space tab
     */
    public void updateWithSpace(Space c, SpaceTab tab) {

        // Save space id
        this.cid = c.getID();
        this.tab = tab;

        // Set id and color of node
        this.setId(cid.toString());
        this.setStyle("-fx-background-color: #FF0000"); // Red color

        // Set up tooltip
        Tooltip tip = new Tooltip("id=" + c.getDisplayName());
        tip.setShowDelay(Duration.seconds(TOOLTIP_DURATION));
        this.setTooltip(tip);

        // Set up event handers
        setupEventHandlers(c);

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // GUI Display
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Draw the space node on the given AnchorPane
     * 
     * @param pane The AnchorPane to draw the space node
     * @param x    The x-coordinate at which to draw
     * @param y    The y-coordinate at which to draw
     */
    public void draw(AnchorPane pane, double x, double y) {
        AnchorPane.setLeftAnchor(this, x - R / 2);
        AnchorPane.setTopAnchor(this, y - R / 2);
        pane.getChildren().add(this);
    }

    /**
     * Draw the space node on the given AnchorPane
     * 
     * @param pane The AnchorPane to draw the space node
     * @param xy   The xy-coordinates at which to draw
     */
    public void draw(AnchorPane pane, double[] xy) {
        draw(pane, xy[0], xy[1]);
    }

    /**
     * Set the shape of the node depending on whether it is a z-neighbor
     * 
     * @param c The space the potentially change shape
     */
    public void setZNeighborShape(Space c) {

        SpacesModel cm   = Model.getSpaces();
        SpaceNode   node = SpaceTab.nodes.get(c.getID());

        // Check if neighbor on lower/higher floor exists
        boolean lo = false, hi = false;
        for (Integer n : c.getNeighbors()) {
            Space nc = cm.get(n);
            lo = lo || nc.getZ() < c.getZ();
            hi = hi || nc.getZ() > c.getZ();
        }

        // Set shape accordingly
        if (lo && hi) {
            node.setShape(UP_DOWN_POLY);
        } 
        else if (lo) {
            node.setShape(DOWN_POLY);
        } 
        else if (hi) {
            node.setShape(UP_POLY);
        }
        else {
            node.setShape(R_CIRCLE);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // View Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Set up all the event handlers necessary for a single node
     * 
     * @param c The space associated with the node
     */
    private void setupEventHandlers(Space c) {
        Backdrop          bd     = tab.getBackdrop();
        CoordinateManager coords = bd.getCoordinateManager();

        // If mouse clicks on node, then we start to make an edge
        this.setOnMousePressed(e -> {
            if (dragSpace == null)
                dragSpace = c;
        });

        // If mouse drags, then draw a line
        this.setOnMouseDragged(e -> {

            // No node was clicked
            if (dragSpace == null)
                return;

            double[] xyPixel = coords.nodeToPixel(e.getX(), e.getY(), c);
            double[] xy      = coords.pixelToUser(xyPixel);

            if (coords.containsUserCoordinates(xy)) {
                // Draw image and graph (without nodes)
                tab.update(false);

                // If within boundaries of another node, then snap to it
                Space snap = tab.getClosestNode(xy, coords.Rxy);
                if (snap != null && snap != dragSpace) {
                    xyPixel = coords.userToPixel(snap.getCoordinates());
                    View.setStatus("Space " + dragSpace.getID() + 
                            " <-> " + snap.getID());
                } else {
                    View.setStatus("Space " + dragSpace.getID() + 
                            " <-> ...");
                }

                // Draw line
                double[] dragCoords = dragSpace.getCoordinates();
                double[] xyTrans    = coords.userToPixel(dragCoords);
                tab.drawLine(xyTrans, xyPixel);
            } 
            else {
                View.setStatus("Move cursor inside image");
            }
        });

        // If mouse released, then add neighbor or remove edge
        this.setOnMouseReleased(e -> {
            if (dragSpace == null)
                return;

            double[] xyPixel = coords.nodeToPixel(e.getX(), e.getY(), c);
            double[] xy      = coords.pixelToUser(xyPixel);
            double[] xyRound = coords.roundCoordinates(xy);

            if (coords.containsUserCoordinates(xy)) {
                Space target = tab.getClosestNode(xy, coords.Rxy);

                // valid target node was found
                if (target != null && target != dragSpace) {

                    // Add neighbor if does not exist
                    if (!target.getNeighbors().contains(dragSpace.getID())) {
                        target.addNeighbor(dragSpace.getID());
                        dragSpace.addNeighbor(target.getID());
                        View.log("Added Neighbors " + dragSpace.getID() + 
                                " <-> " + target.getID());
                    }
                    // Remove neighbor if it already exists
                    else {
                        target.removeNeighbor(dragSpace.getID());
                        dragSpace.removeNeighbor(target.getID());
                        View.log("Removed Neighbors " + dragSpace.getID() + 
                                " <-> " + target.getID());
                    }
                }

                dragSpace = null;

                View.setStatus("(" + xyRound[0] + ", " + xyRound[1] + ")");
            } 
            else {
                View.setStatus("Move cursor inside image");
            }

            tab.update(true);
        });

        // Add a context menu for the node's right-click options
        this.setOnContextMenuRequested(e -> {
            ContextMenu menu = makeNodeContextMenu(e.getX(), e.getY(), c);
            tab.showContextMenu(menu, e.getScreenX(), e.getScreenY());
        });

    }

    /**
     * Create a context menu for a node that was right-clicked
     * 
     * @param x The x-coordinate at which to display
     * @param y The y-coordinate at which to display
     * @param c The space associated with the node
     * @return A context menu for a node that was right-clicked
     */
    private ContextMenu makeNodeContextMenu(double x, double y, Space c) {
        ContextMenu menu = new ContextMenu();

        // Editing the selected space
        MenuItem editSpace = new MenuItem("Edit space " + c.getID());
        editSpace.setOnAction(e -> new SpaceForm(c).show());

        // Deleting the selected space
        MenuItem deleteSpace = new MenuItem("Delete space " + c.getID());
        deleteSpace.setOnAction(e -> {
            tab.removeSpaceNode(c);
            tab.update(true);
        });

        // Adding a neighbor on a different floor
        MenuItem addZNeighbor = new MenuItem("Add new z-neighbor");
        addZNeighbor.setOnAction(e -> {

            SpacesModel cm = Model.getSpaces();

            // Get sorted list of non-adjacent spaces on different floors
            IDChoiceBox neighborChoice = new IDChoiceBox(cm.stream(),
                    n -> (!c.getFloor().equals(((Space) n).getFloor())) && 
                         (!c.getNeighbors().contains(n.getID())));

            // Create form to select a Z-neighbor
            Form form = new Form();
            form.addEntry("Add Z-Neighbor:", neighborChoice);
            form.addSave(e2 -> {
                Integer id = neighborChoice.getSelectedID();
                if (id == null)
                    throw new IllegalArgumentException("Empty z-neighbor");

                // Get space and add as neighbors
                Space zc = cm.get(id);
                zc.addNeighbor(c.getID());
                c.addNeighbor(zc.getID());

                // Set the appropriate shape
                setZNeighborShape(c);
                setZNeighborShape(zc);

                View.log("Added z-neighbor: " + c.getID() + " <-> " + id);
            });
            form.show();

        });

        // Removing a z-neighbor from a different floor
        MenuItem deleteZNeighbor = new MenuItem("Delete z-neighbor...");
        deleteZNeighbor.setOnAction(e -> {

            SpacesModel cm = Model.getSpaces();

            // Get sorted list of z-neighboring spaces
            IDChoiceBox neighborChoice = new IDChoiceBox(
                    cm.get(c.getID()).getNeighbors().stream()
                        .map(o -> cm.get(o))
                        .filter(o -> !c.getFloor().equals(o.getFloor())));

            // Create form to select a Z-neighbor
            Form form = new Form();
            form.addEntry("Remove Z-Neighbor:", neighborChoice);
            form.addSave(e2 -> {
                Integer id = neighborChoice.getSelectedID();
                if (id == null)
                    throw new IllegalArgumentException("Empty z-neighbor");

                // Get space and remove as neighbors
                Space zc = Model.getSpaces().get(id);
                zc.removeNeighbor(c.getID());
                c.removeNeighbor(zc.getID());

                // Set the appropriate shape
                setZNeighborShape(c);
                setZNeighborShape(zc);

                View.log("Removed z-neighbor: " + c.getID() + " - " + id);
            });
            form.show();
        });

        // Adding a new event to the space
        MenuItem addEditEvent = new MenuItem("Add/Edit event");
        addEditEvent.setOnAction(e -> new EventForm(c).show());
        
        // Add a new sensor to the space
        MenuItem addEditSensor = new MenuItem("Add/Edit sensor");
        addEditSensor.setOnAction(e -> new SensorForm(c).show());

        menu.getItems().addAll(
                editSpace, 
                deleteSpace, 
                addZNeighbor, 
                deleteZNeighbor, 
                addEditEvent,
                addEditSensor);

        return menu;
    }

    

}
