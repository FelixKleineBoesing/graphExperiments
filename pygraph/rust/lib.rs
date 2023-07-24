use pyo3::prelude::*;
use pyo3::types::{IntoPyDict, PyDict};
use std::collections::HashMap;

#[pyclass]
pub struct MultiDiGraph {
    node_data: HashMap<String, i32>,
    edge_data: HashMap<String, HashMap<String, HashMap<String, i32>>>,
}

#[pymethods]
impl MultiDiGraph {

    #[new]
    fn new(node_data: HashMap<String, i32>, edge_data: HashMap<(String, String, String), i32>) -> Self {
        let mut graph = MultiDiGraph {
            node_data,
            edge_data: HashMap::new(),
        };

        for (edge_id, data) in edge_data {
            graph.push_edge_data(edge_id, data);
        }

        graph
    }

    fn push_edge_data(&mut self, edge_id: (String, String, String), data: i32) {
        if !self.edge_data.contains_key(&edge_id.0) {
            self.edge_data.insert(edge_id.0.clone(), HashMap::new());
        }

        let edge_data_lvl1 = self.edge_data.get_mut(&edge_id.0).unwrap();

        if !edge_data_lvl1.contains_key(&edge_id.1) {
            edge_data_lvl1.insert(edge_id.1.clone(), HashMap::new());
        }

        let edge_data_lvl2 = edge_data_lvl1.get_mut(&edge_id.1).unwrap();

        edge_data_lvl2.insert(edge_id.2, data);
    }

    fn get(&self, py: Python, item: &str) -> PyResult<Option<PyObject>> {
        match self.edge_data.get(item) {
            Some(inner_map) => {
                let py_map = PyDict::new(py);

                for (k, v) in inner_map.iter() {
                    let inner_py_map = PyDict::new(py);

                    for (k_inner, v_inner) in v.iter() {
                        inner_py_map.set_item(k_inner, *v_inner)?;
                    }

                    py_map.set_item(k, inner_py_map)?;
                }

                Ok(Some(py_map.into()))
            }
            None => Ok(None)
        }
    }

    fn loop_get_item(&self, number_loops: i32) {
        for _ in 0..number_loops {
            let _ = self.edge_data.get("a").unwrap().get("b").unwrap().get("99");
        }
    }

    fn loop_set_item(&mut self, number_loops: i32) {
        for _ in 0..number_loops {
            if let Some(edge_data_lvl1) = self.edge_data.get_mut("a") {
                if let Some(edge_data_lvl2) = edge_data_lvl1.get_mut("b") {
                    edge_data_lvl2.insert("99".to_string(), 99);
                }
            }
        }
    }
}

#[pymodule]
fn graphlib(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_class::<MultiDiGraph>()?;
    Ok(())
}

