package modelutils;

import java.util.HashMap;

/**
 * A HashMap that uses doubles as keys
 */
public class DoubleKeyMap<V> extends HashMap<Integer, V> {

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Used for Serialization
     */
    private static final long serialVersionUID = -1379782250292320466L;

    /**
     * Multiplier to round doubles to integers
     */
    public static final int ROUND = 1000;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    // Queries / Access Methods
    ////////////////////////////////////////////////////////////////////////////

    /**
     * Return true if the key exists in the map
     *
     * @param key The key to access the map
     * @return True if the value associated with the given key exists
     */
    @Override
    public boolean containsKey(Object key) {
        int k = (int) Math.round((double) key * ROUND);
        return super.containsKey(k);
    }

    /**
     * Return the value associated with a key in the map
     *
     * @param key The key to access the map
     * @return The value associated with the given key
     */
    @Override
    public V get(Object key) {
        int k = (int) Math.round((double) key * ROUND);
        return super.get(k);
    }

    /**
     * Put the key-value pair in the map
     *
     * @param key   The key to insert into the map
     * @param value The value to insert into the map
     * @return The previous value associated with the key, if it exists
     */
    @Override
    public V put(Integer key, V value) {
        return this.put((double) key, value);
    }

    /**
     * Put the key-value pair in the map
     *
     * @param key   The key to insert into the map
     * @param value The value to insert into the map
     * @return The previous value associated with the key, if it exists
     */
    public V put(double key, V value) {
        int k = (int) Math.round(key * ROUND);
        return super.put(k, value);
    }

    /**
     * Remove the key-value pair with the given key
     *
     * @param key The key to remove from the map
     * @return The previous value associated with the key, if it exists
     */
    @Override
    public V remove(Object key) {
        int k = (int) Math.round((double) key * ROUND);
        return super.remove(k);
    }

}
