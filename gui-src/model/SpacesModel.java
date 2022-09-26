package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.stream.Stream;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import modelutils.ErrorStringBuilder;
import view.View;

public class SpacesModel extends EntityModel<SpacesModel.Space> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of SpacesModel class
     */
    private static final SpacesModel instance = new SpacesModel();

    /**
     * Return an instance of the SpacesModel class
     *
     * @return An instance of the SpacesModel class
     */
    public static SpacesModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private SpacesModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Space Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class Space extends EntityModel<SpacesModel.Space>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Double       x         = null;
        private Double       y         = null;
        private Double       z         = null;
        private String       floor     = null;
        private Integer      capacity  = -1;
        private Set<Integer> neighbors = new HashSet<>();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return the coordinates of the space
         * 
         * @return The coordinates of the space
         */
        public double[] getCoordinates() {
            return new double[] { x, y, z };
        }

        /**
         * Return the x-coordinate of the space
         * 
         * @return The x-coordinate of the space
         */
        public Double getX() {
            return x;
        }

        /**
         * Return the y-coordinate of the space
         * 
         * @return The y-coordinate of the space
         */
        public Double getY() {
            return y;
        }

        /**
         * Return the z-coordinate of the space
         * 
         * @return The z-coordinate of the space
         */
        public Double getZ() {
            return z;
        }

        /**
         * Return the name of the floor on which the space exists
         * 
         * @return The name of the floor
         */
        public String getFloor() {
            return floor;
        }

        /**
         * Return the capacity of the space
         * 
         * @return The capacity of the space
         */
        public Integer getCapacity() {
            return capacity;
        }

        /**
         * Return the set of neighboring spaces
         * 
         * @return The set of neighboring spaces
         */
        public Set<Integer> getNeighbors() {
            return neighbors;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the coordinates of the space
         * 
         * @param x The x-coordinate of the space
         * @param y The y-coordinate of the space
         * @param z The z-coordinate of the space
         */
        public void setCoordinates(Double x, Double y, Double z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        /**
         * Set the coordinates of the space
         * 
         * @param xyz The xyz-coordinates of the space
         */
        public void setCoordinates(Double[] xyz) {
            this.x = xyz[0];
            this.y = xyz[1];
            this.z = xyz[2];
        }

        /**
         * Set the x-coordinate of the space
         * 
         * @param x The x-coordinate of the space
         */
        public void setX(Double x) {
            this.x = x;
        }

        /**
         * Set the y-coordinate of the space
         * 
         * @param y The y-coordinate of the space
         */
        public void setY(Double y) {
            this.y = y;
        }

        /**
         * Set the z-coordinate of the space
         * 
         * @param z The z-coordinate of the space
         */
        public void setZ(Double z) {
            this.z = z;
        }

        /**
         * Set the name of the floor on which the space exists
         * 
         * @param floor The name of the floor
         */
        public void setFloor(String floor) {
            this.floor = floor;
        }

        /**
         * Set the capacity of the space
         * 
         * @param capacity The capacity of the space
         */
        public void setCapacity(Integer capacity) {
            this.capacity = capacity;
        }

        /**
         * Add a neighboring space
         * 
         * @param id The space ID to add
         */
        public void addNeighbor(Integer id) {
            neighbors.add(id);
        }

        /**
         * Remove a neighboring space
         * 
         * @param id The space ID to remove
         */
        public void removeNeighbor(Integer id) {
            neighbors.remove(id);
        }


        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the space is valid
         * 
         * @return True if the space is valid
         */
        public boolean isValid() {
            return id != null && 
                    x != null && y != null && z != null && 
                    !neighbors.isEmpty();
        }

        /**
         * Return the distance from this space to the provided xy coordinates
         * 
         * @param x An x-coordinate
         * @param y A y-coordinate
         * @return The distance between this space and the provided coordinates
         */
        public double dist(double x, double y) {
            return Math.sqrt(
                    (x - this.x) * (x - this.x) + 
                    (y - this.y) * (y - this.y));
        }

        /**
         * Return the distance from this space to the provided xyz coordinates
         * 
         * @param x An x-coordinate
         * @param y A y-coordinate
         * @param z A z-coordinate
         * @return The distance between this space and the provided coordinates
         */
        public double dist(double x, double y, double z) {
            return Math.sqrt(
                    (x - this.x) * (x - this.x) + 
                    (y - this.y) * (y - this.y) + 
                    (z - this.z) * (z - this.z));
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Iterators / Streams
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return an iterator over the spaces on the given floor
     * 
     * @param floor The name of the floor
     * @return An iterator over the spaces on the given floor
     */
    public Iterator<Space> iterator(String floor) {
        return this.stream(floor).iterator();
    }

    /**
     * Return a stream over the spaces on the given floor
     * 
     * @param floor The name of the floor
     * @return A stream over the spaces on the given floor
     */
    public Stream<Space> stream(String floor) {
        return super.stream().filter(x -> floor.equals(x.floor));
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a space from the model
     * 
     * @param c The space to remove
     */
    public void remove(Space c) {
        // remove neighbors
        for (Integer n : c.neighbors)
            idMap.get(n).removeNeighbor(c.id);

        // remove node
        idMap.remove(c.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Other Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return the space closest to the xy coordinate +/- rxy on the given floor
     * 
     * @param xy    The query xy coordinate
     * @param floor The name of the floor on which to search
     * @param rxy   The maximum radius allowed
     * @return The space closest to xy +/- rxy on the given floor
     */
    public Space find(double[] xy, String floor, double[] rxy) {
        return find(xy[0], xy[1], floor, rxy[0], rxy[1]);
    }

    /**
     * Return the space closest to the xy coordinate +/- rxy on the given floor
     * 
     * @param xy    The query xy coordinate
     * @param floor The name of the floor on which to search
     * @param rx    The maximum radius (in x) allowed
     * @param ry    The maximum radius (in y) allowed
     * @return The space closest to xy +/- rxy on the given floor
     */
    public Space find(double[] xy, String floor, double rx, double ry) {
        return find(xy[0], xy[1], floor, rx, ry);
    }

    /**
     * Return the space closest to the xy coordinate +/- rxy on the given floor
     * 
     * @param x     The query x coordinate
     * @param y     The query y coordinate
     * @param floor The name of the floor on which to search
     * @param rx    The maximum radius (in x) allowed
     * @param ry    The maximum radius (in y) allowed
     * @return The space closest to xy +/- rxy on the given floor
     */
    public Space find(double x, double y, String floor, double rx, double ry) {
        Space closest = this.stream(floor)
                            .min((c1, c2) -> 
                                Double.compare(c1.dist(x, y), c2.dist(x, y)))
                            .orElse(null);

        // If no space found, return null
        if (closest == null)
            return null;

        // Check that space is within specified rx, ry ellipse
        double p = ((x - closest.x) * (x - closest.x) / (rx * rx)) + 
                   ((y - closest.y) * (y - closest.y) / (ry * ry));
        if (p <= 1.0)
            return closest;

        return null;
    }

    /**
     * Return true if the spaces model and backdrop model are valid
     * 
     * @param print Set to true to print errors
     * @return True if the spaces model and backdrop model are valid
     */
    public boolean checkValid(boolean print) {
        
        super.checkValid(print);

        ErrorStringBuilder err = new ErrorStringBuilder();

        // Space 0 must exist
        if (!containsID(0))
            err.append("Must have special space 0 (outside)");

        if (print && !err.isEmpty())
            View.log(err.toString());

        // if empty, no errors were found
        return err.isEmpty();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Load the provided spaces file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No spaces file selected");
            return;
        }

        // To obtain relevant floors
        BackdropModel bm = Model.getBackdrop();

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                Space c = new Space();

                // id
                c.setID((int) (long) o.get("id"));

                // description
                c.setDescription((String) o.get("description"));

                // coordinates
                JSONArray coordinates = (JSONArray) o.get("coordinates");
                c.setX((double) coordinates.get(0));
                c.setY((double) coordinates.get(1));
                c.setZ((double) coordinates.get(2));

                // floor
                c.setFloor(bm.getFloorFromZ(c.getZ()));

                // capacity
                c.setCapacity((int) (long) o.get("capacity"));

                // neighbors
                JSONArray neighbors = (JSONArray) o.get("neighbors");
                for (Object neighbor : neighbors)
                    c.addNeighbor((int) (long) neighbor);

                this.add(c);
            }

            View.log("Loaded Spaces File: " + f.getAbsolutePath());
        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid spaces file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the spaces model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getSpacesPath();

        JSONArray data = new JSONArray();

        for (Space c : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", c.id);

            // description
            o.put("description", c.description);

            // coordinates
            JSONArray coordinates = new JSONArray();
            coordinates.add(c.x);
            coordinates.add(c.y);
            coordinates.add(c.z);
            o.put("coordinates", coordinates);

            // capacity
            o.put("capacity", c.capacity);

            // neighbors
            JSONArray neighbors = new JSONArray();
            for (Integer x : c.neighbors)
                neighbors.add(x);
            o.put("neighbors", neighbors);

            data.add(o);
        }
        
        Model.getInstance().writeJSON(fname, data, "spaces");

    }

}
