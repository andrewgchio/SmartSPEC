package model;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import view.View;

public class SensorsModel extends EntityModel<SensorsModel.Sensor> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Singleton Construction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Singleton instance of SensorsModel class
     */
    private static final SensorsModel instance = new SensorsModel();

    /**
     * Return an instance of the SensorsModel class
     *
     * @return An instance of the SensorsModel class
     */
    public static SensorsModel getInstance() { return instance; }

    /**
     * Private default constructor for singleton
     */
    private SensorsModel() {}

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Sensor Class Definition
    ////////////////////////////////////////////////////////////////////////////

    public class Sensor extends EntityModel<SensorsModel.Sensor>.Entity {

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Instance Variables
        ////////////////////////////////////////////////////////////////////////

        private Integer      metaid   = -1;
        private Double       x        = null;
        private Double       y        = null;
        private Double       z        = null;
        private Set<Integer> coverage = new HashSet<>();

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Getter Methods
        ////////////////////////////////////////////////////////////////////////
        
        /**
         * Return the metasensor ID
         * 
         * @return The metasensor ID
         */
        public int getMetaID() {
            return this.metaid;
        }

        /**
         * Return the coordinates of the sensor
         * 
         * @return The coordinates of the sensor
         */
        public double[] getCoordinates() {
            return new double[] { x, y, z };
        }

        /**
         * Return the x-coordinate of the sensor
         * 
         * @return The x-coordinate of the sensor
         */
        public Double getX() {
            return x;
        }

        /**
         * Return the y-coordinate of the sensor
         * 
         * @return The y-coordinate of the sensor
         */
        public Double getY() {
            return y;
        }

        /**
         * Return the z-coordinate of the sensor
         * 
         * @return The z-coordinate of the sensor
         */
        public Double getZ() {
            return z;
        }

        /**
         * Return the coverage of the sensor
         * 
         * @return The coverage of the sensor
         */
        public Set<Integer> getCoverage() {
            return coverage;
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Setter Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Set the metasensor ID
         * 
         * @param id The metasensor ID
         */
        public void setMetaID(Integer id) {
            this.metaid = id;
        }

        /**
         * Set the coordinates of the sensor
         * 
         * @param x The x-coordinate of the sensor
         * @param y The y-coordinate of the sensor
         * @param z The z-coordinate of the sensor
         */
        public void setCoordinates(Double x, Double y, Double z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        /**
         * Set the coordinates of the sensor
         * 
         * @param xyz The xyz-coordinates of the sensor
         */
        public void setCoordinates(Double[] xyz) {
            this.x = xyz[0];
            this.y = xyz[1];
            this.z = xyz[2];
        }

        /**
         * Set the x-coordinate of the sensor
         * 
         * @param x The x-coordinate of the sensor
         */
        public void setX(Double x) {
            this.x = x;
        }

        /**
         * Set the y-coordinate of the sensor
         * 
         * @param y The y-coordinate of the sensor
         */
        public void setY(Double y) {
            this.y = y;
        }

        /**
         * Set the z-coordinate of the sensor
         * 
         * @param z The z-coordinate of the sensor
         */
        public void setZ(Double z) {
            this.z = z;
        }

        /**
         * Add a covered space
         * 
         * @param id The space ID to add
         */
        public void addCoverage(Integer id) {
            coverage.add(id);
        }

        /**
         * Remove a covered space
         * 
         * @param id The space ID to remove
         */
        public void removeCoverage(Integer id) {
            coverage.remove(id);
        }

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        // Other Methods
        ////////////////////////////////////////////////////////////////////////

        /**
         * Return true if the sensor is valid
         * 
         * @return True if the sensor is valid
         */
        public boolean isValid() {
            return id != null && 
                    x != null && y != null && z != null && 
                    !coverage.isEmpty();
        }

    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Remove a sensor from the model
     * 
     * @param s The sensor to remove
     */
    public void remove(Sensor s) {
        idMap.remove(s.id);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // I/O Methods
    ////////////////////////////////////////////////////////////////////////////
    
    
    /**
     * Load the provided events file
     * 
     * @param f The file to load
     */
    public void loadFile(File f) {
        if (f == null) {
            View.log("Error: No sensors file selected");
            return;
        }

        try {
            JSONParser parser = new JSONParser();

            JSONArray arr = (JSONArray) parser.parse(new FileReader(f));
            for (Object element : arr) {
                JSONObject o = (JSONObject) element;

                Sensor s = new Sensor();

                // id
                s.setID((int) (long) o.get("id"));

                // metaevent-id
                s.setMetaID((int) (long) o.get("metasensors-id"));

                // description
                s.setDescription((String) o.get("description"));
                
                // coordinates
                JSONArray coordinates = (JSONArray) o.get("coordinates");
                s.setX((double) coordinates.get(0));
                s.setY((double) coordinates.get(1));
                s.setZ((double) coordinates.get(2)); 

                // coverage
                JSONArray spaceIDs = (JSONArray) o.get("coverage");
                for (Object spaceID : spaceIDs)
                    s.addCoverage((int) (long) spaceID);

                this.add(s);
            }

            View.log("Loaded Sensors File: " + f.getAbsolutePath());

        } 
        catch (IllegalArgumentException e) {
            View.log("Error: " + e.getMessage());
        } 
        catch (ParseException e) {
            View.log("Error: Cannot parse file: " + f.getAbsolutePath());
        } 
        catch (IOException e) {
            View.log("Error: Invalid sensors file: " + f.getAbsolutePath());
        }
    }

    /**
     * Write the sensors model to file
     */
    @SuppressWarnings("unchecked")
    public void writeToFile() {
        if (!checkValid(true))
            return;

        String fname = Model.getConfig().getSensorsPath();

        JSONArray data = new JSONArray();

        for (Sensor s : this) {
            JSONObject o = new JSONObject();

            // id
            o.put("id", s.getID());

            // metaevent-id
            o.put("metasensor-id", s.getMetaID());

            // description
            o.put("description", s.getDescription());
            
            // coordinates
            JSONArray coordinates = new JSONArray();
            coordinates.add(s.x);
            coordinates.add(s.y);
            coordinates.add(s.z);
            o.put("coordinates", coordinates);

            // coverage
            JSONArray spaceIDs = new JSONArray();
            for (Integer x : s.getCoverage())
                spaceIDs.add(x);
            o.put("coverage", spaceIDs);

            data.add(o);
        }

        Model.getInstance().writeJSON(fname, data, "sensors");

    }

}
