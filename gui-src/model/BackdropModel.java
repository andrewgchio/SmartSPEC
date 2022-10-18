package model;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.stream.Stream;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import javafx.scene.image.Image;
import modelutils.CoordinateManager;
import modelutils.DoubleKeyMap;
import view.View;

public class BackdropModel implements Iterable<model.BackdropModel.Backdrop> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of BackdropModel class
     */
    private static final BackdropModel instance = new BackdropModel();

    /**
     * Return an instance of the BackdropModel class
     * 
     * @return An instance of the BackdropModel class
     */
    public static BackdropModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private BackdropModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Instance Variables
    ////////////////////////////////////////////////////////////////////////////

    /**
     * A map of backdrop floor names to backdrops
     */
    private Map<String, Backdrop> backdrops = new HashMap<>();

    /**
     * A map of z coordinates to floors
     */
    private DoubleKeyMap<String> zToFloorMap = new DoubleKeyMap<>();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /*
     * Percentage of the window that the image should fill
     */
    public static final double IMAGE_FILL_PERCENT = 0.8;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Backdrop Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class Backdrop {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private String            floor       = null;
        private Double            z           = null;
        private String            imagePath   = null;
        private Image             image       = null;
        private CoordinateManager coordinates = new CoordinateManager();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the floor of the backdrop
         * 
         * @return The floor of the backdrop
         */
        public String getFloor() {
            return floor;
        }

        /**
         * Return the z-coordinate of the backdrop
         * 
         * @return The z-coordinate of the backdrop
         */
        public Double getZ() {
            return z;
        }

        /**
         * Return the image filepath of the backdrop
         * 
         * @return The image filepath of the backdrop
         */
        public String getImagePath() {
            return imagePath;
        }

        /**
         * Return the backdrop image
         * 
         * @return The backdrop image
         */
        public Image getImage() {
            return image;
        }

        /**
         * Return the coordinate manager for the backdrop
         * 
         * @return The coordinate manager for the backdrop
         */
        public CoordinateManager getCoordinateManager() {
            return coordinates;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the floor of the backdrop
         * 
         * @param floor The floor of the backdrop
         */
        public void setFloor(String floor) {
            this.floor = floor;
        }

        /**
         * Set the z-coordinate of the backdrop
         * 
         * @param z The z-coordinate of the backdrop
         */
        public void setZ(Double z) {
            this.z = z;
        }

        /**
         * Set the image for the backdrop
         * 
         * @param path The image file path
         */
        public void setImage(String path) {
            imagePath = path;
            try {
                image = new Image(new FileInputStream(imagePath), 
                        View.WINDOW_WIDTH * IMAGE_FILL_PERCENT,
                        View.WINDOW_HEIGHT * IMAGE_FILL_PERCENT, true, true);
                coordinates.setImageSize(image.getWidth(), image.getHeight());
            } 
            catch (FileNotFoundException e) {
                throw new IllegalArgumentException(
                        "Image file not found: " + imagePath);
            }
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an iterator over the backdrops
     * 
     * @return An iterator over the backdrops
     */
    @Override
    public Iterator<Backdrop> iterator() {
        return backdrops.values().iterator();
    }

    /**
     * Return a stream over the floor names
     * 
     * @return A stream over the floor names
     */
    public Stream<String> streamFloors() {
        return backdrops.keySet().stream().sorted();
    }

    /**
     * Return a stream over the spaces
     * 
     * @return A stream over the spaces
     */
    public Stream<Backdrop> stream() {
        return backdrops.values().stream();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the backdrop on the given floor
     * 
     * @param floor The floor name
     * @return The backdrop on the given floor
     */
    public Backdrop get(String floor) {
        return backdrops.get(floor);
    }

    /**
     * Return the z-coordinate of the given floor
     * 
     * @param floor The floor name
     * @return The z-coordinate of the given floor
     */
    public Double getZ(String floor) {
        Backdrop bd = get(floor);
        if (bd == null)
            return null;
        return bd.getZ();
    }

    /**
     * Return the file path to the backdrop image
     * 
     * @param floor The floor name
     * @return The file path to the backdrop image
     */
    public String getImagePath(String floor) {
        Backdrop bd = get(floor);
        if (bd == null)
            return null;
        return bd.getImagePath();
    }

    /**
     * Return the floor with the given z-coordinate
     * 
     * @param z The query z-coordinate
     * @return The floor with the given z-coordinate
     */
    @SuppressWarnings("unlikely-arg-type")
    public String getFloorFromZ(Double z) {
        return zToFloorMap.get(z);
    }

    /**
     * Add the space backdrop to the model
     * 
     * @param bd The backdrop to add
     */
    public void addSpaceBackdrop(Backdrop bd) {
        backdrops.put(bd.floor, bd);
        zToFloorMap.put(bd.z, bd.floor);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided backdrops file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No spaces backdrop file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                Backdrop bd = new Backdrop();

                // floor
                bd.setFloor((String) o.get("floor"));

                // z coordinate
                bd.setZ((double) o.get("z"));

                // imgPath
                bd.setImage((String) o.get("image"));

                // coordinates
                bd.coordinates.x0 = (double) o.get("x0");
                bd.coordinates.y0 = (double) o.get("y0");
                bd.coordinates.x1 = (double) o.get("x1");
                bd.coordinates.y1 = (double) o.get("y1");

                this.addSpaceBackdrop(bd);
            }

            View.log("Loaded backdrop file: " + f.getAbsolutePath());
        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid backdrop file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the backdrops model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        String fname = Paths.get(Model.getConfig().getBaseDir(), 
                "Backdrops.json").toString();

        JSONArray data = new JSONArray();

        for (Backdrop cb : this) {
            JSONObject o = new JSONObject();

            // floor
            o.put("floor", cb.getFloor());

            // z
            o.put("z", cb.getZ());

            // image path
            o.put("image", cb.getImagePath());

            // coordinates
            o.put("x0", cb.coordinates.x0);
            o.put("y0", cb.coordinates.y0);
            o.put("x1", cb.coordinates.x1);
            o.put("y1", cb.coordinates.y1);

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "backdrops");

    }

}
