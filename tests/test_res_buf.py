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
    buf.push(new_res(1.0))
    buf.push(new_res(2.0))

    rs = buf.pop_all_res()

    assert len(buf) == 0
    assert [type(r) for r in rs] == [res_type, res_type]
    assert [r.quantity for r in rs] == [1.0, 2.0]

    buf.push_many(rs)

    assert len(buf) == 2
    assert buf.quantity == pytest.approx(3.0)


@pytest.mark.parametrize("buf_type,res_type,new_res,new_other_res",
                         RES_BUF_CASES)
def test_push_wrong_type(buf_type, res_type, new_res, new_other_res):
    """Tests that a buffer only accepts resources of its type."""
    buf = buf_type(init=True)
    buf.capacity = 10.0

    with pytest.raises(TypeError):
        buf.push(new_other_res(1.0))
    with pytest.raises(TypeError):
        buf.push_many([new_other_res(1.0)])

    assert len(buf) == 0


@pytest.mark.parametrize("res_type,new_res", [(ts.Material, new_material),
                                              (ts.Product, new_product)])
def test_extract_res_keeps_type(res_type, new_res):
    """Tests that extract_res() returns a resource of the type of the
    resource that it is called on.
    """
    r = new_res(2.0)
    assert type(r.extract_res(1.0)) is res_type
