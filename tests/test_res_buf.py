"""Tests the Python wrappers of the resource buffers."""
import pytest

from cyclus import typesystem as ts


def new_material(qty):
    return ts.Material.create_untracked(qty, {"H1": 1})


def new_product(qty):
    return ts.Product.create_untracked(qty, "quality")


# The type of buffer, the type of resource that it holds, and functions that
# create a resource of that type and one of the other type
RES_BUF_CASES = [
    (ts.ResBufMaterial, ts.Material, new_material, new_product),
    (ts.ResBufProduct, ts.Product, new_product, new_material),
]


@pytest.mark.parametrize("buf_type,res_type,new_res,new_other_res",
                         RES_BUF_CASES)
def test_pop_all_res_push_many(buf_type, res_type, new_res, new_other_res):
    """Tests that the resources of a buffer can be popped and pushed back, as
    is done to snapshot an inventory, and that they have the type of the
    buffer, as they do in C++.
    """
    buf = buf_type(init=True)
    buf.capacity = 10.0
    qty1 = 1.0
    qty2 = 2.0
    buf.push(new_res(qty1))
    buf.push(new_res(qty2))

    popped = buf.pop_all_res()

    assert len(buf) == 0
    assert [type(res) for res in popped] == [res_type, res_type]
    assert [res.quantity for res in popped] == pytest.approx([qty1, qty2])

    buf.push_many(popped)

    assert len(buf) == 2
    assert buf.quantity == pytest.approx(qty1 + qty2)


@pytest.mark.parametrize("map_type,res_type,new_res",
                         [(ts.ResMapIntMaterial, ts.Material, new_material),
                          (ts.ResMapIntProduct, ts.Product, new_product)])
def test_res_map_res_values_keep_type(map_type, res_type, new_res):
    """Tests that res_values() and pop_all_res() of a resource map return
    resources of the type that the map holds.
    """
    res_map = map_type(init=True)
    res_map[1] = new_res(1.0)
    res_map[2] = new_res(2.0)

    values = res_map.res_values()
    popped = res_map.pop_all_res()

    assert len(values) == 2
    assert all(type(res) is res_type for res in values)
    assert len(popped) == 2
    assert all(type(res) is res_type for res in popped)


@pytest.mark.parametrize("res_type,new_res", [(ts.Material, new_material),
                                              (ts.Product, new_product)])
def test_extract_res_keeps_type(res_type, new_res):
    """Tests that extract_res() returns a resource of the type of the
    resource that it is called on.
    """
    res = new_res(2.0)
    assert type(res.extract_res(1.0)) is res_type


@pytest.mark.parametrize("res_type,new_res", [(ts.Material, new_material),
                                              (ts.Product, new_product)])
def test_clone_keeps_type(res_type, new_res):
    """Tests that clone() returns a copy of the resource of the type of the
    resource that it is called on.
    """
    res = new_res(2.0)
    copy = res.clone()
    assert type(copy) is res_type
    assert copy.quantity == pytest.approx(2.0)
