package view;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import form.SpaceForm;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Tab;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;
import javafx.stage.Window;
import model.BackdropModel.Backdrop;
import model.Model;
import model.SpacesModel;
import model.SpacesModel.Space;
import modelutils.CoordinateManager;

/**
 * A SpaceTab is used to display the backdrop and space nodes for a particular
 * floor in the GUI.
 */
public class SpaceTab extends Tab {

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////

    private Backdrop bd;

    // The main drawn content
    private AnchorPane      pane;
    private Canvas          canvas;
    private GraphicsContext gc;

    /**
     * A map of nodes drawn in space tabs
     */
    public static Map<Integer, SpaceNode> nodes = new HashMap<>();

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////

    /**
     * Construct a SpaceTab based on the given backdrop
     * 
     * @param bd The backdrop
     */
    public SpaceTab(Backdrop bd) {
        super(bd.getFloor());

        this.bd = bd;

        // AnchorPane allows nodes to be placed via offset
        pane = new AnchorPane();

        // Create canvas
        canvas = makeCanvas(bd);

        // Update the canvas
        update(true);

        // Set pane content: ScrollPane with AnchorPane with Canvas
        pane.getChildren().add(canvas);
        super.setContent(new ScrollPane(pane));
    }

    /**
     * Updates the tab, optionally redrawing nodes
     * 
     * @param drawNodes Set to true to redraw nodes
     */
    public void update(boolean drawNodes) {
        // Clear everything
        clear(drawNodes);
        
        // Redraw image
        CoordinateManager coords = bd.getCoordinateManager();
        gc.drawImage(bd.getImage(), coords.imageOffsetX, coords.imageOffsetY);

        // Redraw graph
        SpacesModel cm = Model.getSpaces();
        for (Iterator<Space> it = cm.iterator(bd.getFloor()); it.hasNext();) {
            Space c = it.next();

            // Redraw nodes, adding new space nodes if does not exist
            double[] xyTrans = coords.userToPixel(c.getCoordinates());
            if (drawNodes) {
                SpaceNode node = nodes.computeIfAbsent(c.getID(), 
                        k -> new SpaceNode(c, this));
                node.setZNeighborShape(c);
                node.draw(pane, xyTrans);
            }

            // Redraw edges
            for (Integer n : c.getNeighbors()) {
                Space nc = cm.get(n);
                if (bd.getFloor().equals(nc.getFloor())) {
                    double[] ncCoords  = nc.getCoordinates();
                    double[] xyTransNC = coords.userToPixel(ncCoords);
                    drawLine(xyTrans, xyTransNC);
                }
            }
        }
    }

    /**
     * Clears the space tab. If drawNodes is true, then remove nodes as well.
     * 
     * @param drawNodes Set to true to clear nodes as well
     */
    public void clear(boolean drawNodes) {
        // Remove nodes if redrawing everything
        if (drawNodes)
            pane.getChildren().retainAll(canvas);

        // Set transparency to 70%
        gc.setGlobalAlpha(0.7);

        // Clear image and edges
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }

    /**
     * Return the backdrop associated with the space tab
     * 
     * @return The backdrop associated with the space tab
     */
    public Backdrop getBackdrop() {
        return bd;
    }

    /**
     * Return the node closest to the xy coordinates +/- Rxy
     * 
     * @param xy  The query xy-coordinate
     * @param Rxy The maximum radius allowed
     * @return The node closest to the xy-coordinates +/- Rxy
     */
    public Space getClosestNode(double[] xy, double[] Rxy) {
        return Model.getSpaces().find(xy, bd.getFloor(), Rxy);
    }

    /**
     * Remove a space node from the tab
     * 
     * @param c The space to remove
     */
    public void removeSpaceNode(Space c) {
        Model.getSpaces().remove(c);
        nodes.remove(c.getID());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // GUI Display
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Draw a line on the space tab
     * 
     * @param x0 The x-coordinate of the top left corner
     * @param y0 The y-coordinate of the top left corner
     * @param x1 The x-coordinate of the bottom right corner
     * @param y1 The y-coordinate of the bottom right corner
     */
    public void drawLine(double x0, double y0, double x1, double y1) {
        gc.setStroke(Color.RED);
        gc.strokeLine(x0, y0, x1, y1);
    }

    /**
     * Draw a line on the space tab
     * 
     * @param xy0 The xy-coordinate of the top left corner
     * @param xy1 The xy-coordinate of the bottom right corner
     */
    public void drawLine(double[] xy0, double[] xy1) {
        drawLine(xy0[0], xy0[1], xy1[0], xy1[1]);
    }

    /**
     * Display a given context menu at the xy coordinates
     * 
     * @param menu The menu to display
     * @param x    The display x-coordinate
     * @param y    The display y-coordinate
     */
    public void showContextMenu(ContextMenu menu, double x, double y) {
        menu.show(canvas.getScene().getWindow(), x, y);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // View Construction Helpers
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Initializes and returns the canvas based on the provided backdrop
     * 
     * @param bd The backdrop to visualize
     * @return The canvas based on the provided backdrop
     */
    private Canvas makeCanvas(Backdrop bd) {
        // Cache width, height of tab pane
        double width  = View.getInstance().getTabs().getWidth();
        double height = View.getInstance().getTabs().getHeight();

        // Create a new canvas with the specified width, height
        canvas = new Canvas(width, height);
        gc     = canvas.getGraphicsContext2D();

        // Update coordinate manager
        CoordinateManager coords = bd.getCoordinateManager();
        coords.computeUtilityValues(width, height, SpaceNode.R);

        // If mouse is within image, display coordinates
        canvas.setOnMouseMoved(e -> {
            double[] xy = coords.pixelToUser(e.getX(), e.getY());
            if (coords.containsUserCoordinates(xy)) {
                double[] xyRound = coords.roundCoordinates(xy);
                View.setStatus("(" + xyRound[0] + ", " + xyRound[1] + ")");
            } 
            else {
                View.setStatus("-");
            }
        });

        // If right click on image, open context menu to add space
        canvas.setOnContextMenuRequested(e -> {
            double[] xy = coords.pixelToUser(e.getX(), e.getY());
            if (coords.containsUserCoordinates(xy)) {
                ContextMenu menu   = makeCanvasContextMenu(e.getX(), e.getY());
                Window      window = canvas.getScene().getWindow();
                menu.show(window, e.getScreenX(), e.getScreenY());
            }
        });

        return canvas;
    }

    /**
     * Create a context menu to display on right-clicking the image
     * 
     * @param x The display x-coordinate
     * @param y The display y-coordinate
     * @return A context menu to display on right-clicking the image
     */
    private ContextMenu makeCanvasContextMenu(double x, double y) {

        // Create a new context menu
        ContextMenu menu = new ContextMenu();

        // Menu option to add a new space
        MenuItem addSpace = new MenuItem("Add new space");
        addSpace.setOnAction(e -> {
            // Ignore right click if dragging
            if (SpaceNode.dragSpace != null)
                return;

            // Translate pixel coordinates into user coordinates
            CoordinateManager coords = bd.getCoordinateManager();
            double[]          xy     = coords.pixelToUser(x, y);

            // Check if an existing space was clicked
            SpacesModel cm = Model.getSpaces();
            Space       c  = cm.find(xy, bd.getFloor(), coords.Rxy);
            if (c == null) {
                // Create a new node
                SpaceNode node = new SpaceNode();
                node.draw(pane, x, y);

                // Open a space form with the specified coordinates
                SpaceForm entry = new SpaceForm();
                entry.setIsNew(false);
                entry.setCoordinates(xy[0], xy[1], bd.getZ());
                entry.show();
                
                if (entry.isFailed()) {
                    pane.getChildren().remove(node);
                } 
                else {
                    // put node
                    node.updateWithSpace(cm.get(entry.getID()), this);
                    nodes.put(entry.getID(), node);
                }
            }

        });

        // Add menu items to context menu
        menu.getItems().addAll(addSpace);

        return menu;
    }

}