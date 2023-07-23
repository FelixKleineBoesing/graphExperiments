use cpython::{PyResult, PyDict, PyTuple, PyObject,py_class, py_module_initializer};
use std::collections::HashMap;
use std::cell::RefCell;

#[derive(Default)]
pub struct MultiDiGraph {
    node_data: HashMap<String, i32>,
    edge_data: HashMap<String, HashMap<String, HashMap<String, i32>>>,
}

impl MultiDiGraph {
    fn new(node_data: HashMap<String, i32>, edge_data: Vec<(String, String, String, i32)>) -> MultiDiGraph {
        let mut graph = MultiDiGraph {
            node_data,
            edge_data: HashMap::new(),
        };

        for edge in edge_data {
            graph.push_edge_data(edge);
        }

        graph
    }

    fn push_edge_data(&mut self, edge_id: (String, String, String, i32)) {
        let (node1, node2, edge, data) = edge_id;

        if !self.edge_data.contains_key(&node1) {
            self.edge_data.insert(node1.clone(), HashMap::new());
        }
        if !self.edge_data[&node1].contains_key(&node2) {
            self.edge_data.get_mut(&node1).unwrap().insert(node2.clone(), HashMap::new());
        }
        self.edge_data.get_mut(&node1).unwrap().get_mut(&node2).unwrap().insert(edge, data);
    }

    fn get_item(&self, node1: &str, node2: &str, edge: &str) -> Option<i32> {
        self.edge_data.get(node1).and_then(|n| n.get(node2).and_then(|e| e.get(edge).cloned()))
    }

    fn loop_get_item(&self, number_loops: i32) {
        for _ in 0..number_loops {
            self.get_item("a", "b", "99");
        }
    }

    fn loop_set_item(&mut self, number_loops: i32) {
        for _ in 0..number_loops {
            self.push_edge_data(("a".to_string(), "b".to_string(), "99".to_string(), 99));
        }
    }
}

py_class!(pub class PyMultiDiGraph |py| {
    data inner: RefCell<MultiDiGraph>;

    def __new__(_cls, node_data: PyDict, edge_data: PyDict) -> PyResult<PyMultiDiGraph> {
        let mut rust_node_data: HashMap<String, i32> = HashMap::new();
        let mut rust_edge_data: Vec<(String, String, String, i32)> = Vec::new();

        for (key, value) in node_data.items(py) {
            let key: String = key.extract(py)?;
            let value: i32 = value.extract(py)?;
            rust_node_data.insert(key, value);
        }

        for (key, value) in edge_data.items(py) {
            let key: PyTuple = key.extract(py)?;
            let value: i32 = value.extract(py)?;

            let node1: String = key.get_item(py, 0).extract(py)?;
            let node2: String = key.get_item(py, 1).extract(py)?;
            let edge: String = key.get_item(py, 2).extract(py)?;

            rust_edge_data.push((node1, node2, edge, value));
        }

        PyMultiDiGraph::create_instance(py, RefCell::new(MultiDiGraph::new(rust_node_data, rust_edge_data)))
    }

    def loop_get_item(&self, number_loops: PyObject) -> PyResult<PyObject> {
        let loops: i32 = number_loops.extract(py)?;
        self.inner(py).borrow().loop_get_item(loops);
        Ok(py.None())
    }

    def loop_set_item(&self, number_loops: PyObject) -> PyResult<PyObject> {
        let loops: i32 = number_loops.extract(py)?;
        self.inner(py).borrow_mut().loop_set_item(loops);
        Ok(py.None())
    }
});


py_module_initializer!(graphlib, initgraphlib, PyInit_graphlib, |py, m| {
    m.add_class::<PyMultiDiGraph>(py)
});
